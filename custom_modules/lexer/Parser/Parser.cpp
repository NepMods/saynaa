//
// Created by arjun on 7/21/25.
//

#include "Parser.h"


Parser::Parser(Token *tk_list) {
    all_tokens = tk_list;
    index = 0;          // Initialize token index
    hadError = false;
}

void Parser::errorAt(Token *token, const std::string message) {
    std::cerr << "[line " << token->line << "] Error";

    if (token->type == TK_EOF) {
        std::cerr << " at end";
    } else if (token->type == TK_ERROR) {
        // Nothing.
    } else {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    std::cerr << ": " << message << std::endl;
    hadError = true;
}

void Parser::error(const std::string message) { errorAt(&previous, message); }

void Parser::errorAtCurrent(const std::string message) {
    errorAt(&current, message);
}

void Parser::advance() {
    previous = current;

    // Prevent reading beyond tokens by stopping at EOF
    for (;;) {
        current = all_tokens[index];
        if (current.type == TK_EOF) break;
        index++;
        if (current.type != TK_ERROR) break;

        errorAtCurrent(current.start);
    }
}

bool Parser::check(TokenType type) { return current.type == type; }

void Parser::consume(TokenType type, const std::string message) {
    if (check(type)) {
        advance();
        return;
    }

    errorAtCurrent(message);
}

bool Parser::match(TokenType type) {
    if (!check(type))
        return false;
    advance();
    return true;
}

void Parser::emitByte(uint32_t byte) {
    bytecode->opcode.push_back(byte);
    bytecode->lines.push_back(previous.line);
}

void Parser::emitBytes(uint32_t byte1, uint32_t byte2) {
    emitByte(byte1);
    emitByte(byte2);
}

void Parser::emitReturn() {
    emitByte(OP_NULL);
    emitByte(OP_RETURN);
}

uint32_t Parser::makeConstant(std::variant<int, std::string> value) {
    bytecode->value.push_back(value);
    uint32_t constant = bytecode->value.size() - 1;

    return constant;
}

int Parser::emitJump(uint8_t instruction) {
    emitByte(instruction);
    // Emit placeholder for jump offset
    emitByte(0xff);
    return bytecode->opcode.size() - 1; // Return offset of jump operand
}

void Parser::patchJump(int offset, int value) {
    int jump = bytecode->opcode.size() - offset - 1;
    if (jump > 0xff) {
        error("Too much code to jump over.");
    }
    bytecode->opcode[offset] = jump;
}

void Parser::emitConstant(std::variant<int, std::string> value) {
    emitBytes(OP_CONSTANT, makeConstant(value));
}

void Parser::endCompiler() {
    if (bytecode->opcode.empty() || bytecode->opcode.back() != OP_RETURN) {
        if (!isMainThere) {
            emitReturn();
        }
    }

#ifdef DEBUG_PRINT_CODE
    if (!hadError) {
        Debug debug(*bytecode);
        debug.disassemble("OPCODE");
    }
#endif
}

uint32_t Parser::identifierConstant(Token *name) {
    bytecode->name.push_back(copyString(name->start, name->length));
    uint32_t constant = bytecode->name.size() - 1;

    return constant;
}

uint32_t Parser::parseVariable(const char *errorMessage) {
    consume(TK_NAME, errorMessage);
    return identifierConstant(&previous);
}

void Parser::defineVariable(uint32_t global) {
    emitBytes(OP_DEFINE_LOCAL, global);
}

void Parser::binary() {
    TokenType operatorType = previous.type;
    ParseRule rule = getRule(operatorType);
    parsePrecedence((Precedence)(rule.precedence + 1));

    switch (operatorType) {
    case TK_NOTEQ:
        emitByte(OP_NEQU);
        break;
    case TK_EQEQ:
        emitByte(OP_EQUAL);
        break;
    case TK_GT:
        emitByte(OP_GREATER);
        break;
    case TK_GTEQ:
        emitBytes(OP_LESS, OP_NOT);
        break;
    case TK_LT:
        emitByte(OP_LESS);
        break;
    case TK_LTEQ:
        emitBytes(OP_GREATER, OP_NOT);
        break;
    case TK_PLUS:
        emitByte(OP_ADD);
        break;
    case TK_MINUS:
        emitByte(OP_SUBTRACT);
        break;
    case TK_STAR:
        emitByte(OP_MULTIPLY);
        break;
    case TK_SLASH:
        emitByte(OP_DIVIDE);
        break;
    default:
        return; // Unreachable.
    }
}

void Parser::call() {
    uint32_t argCount = 0;
    if (!check(TK_RPARAN)) {
        do {
            expression();
            argCount++;
        } while (match(TK_COMMA));
    }
    consume(TK_RPARAN, "Expect ')' after arguments.");
    emitBytes(OP_CALL, argCount);
}

void Parser::literal() {
    switch (previous.type) {
    case TK_FALSE:
        emitByte(OP_FALSE);
        break;
    case TK_NULL:
        emitByte(OP_NULL);
        break;
    case TK_TRUE:
        emitByte(OP_TRUE);
        break;
    default:
        return; // Unreachable.
    }
}

void Parser::grouping() {
    expression();
    consume(TK_RPARAN, "Expect ')' after expression.");
}

void Parser::number() {
    double value = strtod(previous.start, nullptr);
    emitConstant((int)value);  // adjust if you want to support floats
}

void Parser::string() {
    emitConstant(copyString(previous.start + 1, previous.length - 2));
}

void Parser::namedVariable(Token name) {
    uint32_t arg = identifierConstant(&name);
    if (match(TK_EQ)) {
        expression();
        emitBytes(OP_SET_LOCAL, arg);
    } else {
        emitBytes(OP_GET_LOCAL, arg);
    }
}

void Parser::variable() { namedVariable(previous); }

void Parser::unary() {
    TokenType operatorType = previous.type;

    // Compile the operand.
    parsePrecedence(PREC_UNARY);

    // Emit the operator instruction.
    switch (operatorType) {
    case TK_NOT:
        emitByte(OP_NOT);
        break;
    case TK_MINUS:
        emitByte(OP_NEGATE);
        break;
    default:
        return; // Unreachable.
    }
}

void Parser::parsePrecedence(Precedence precedence) {
    advance();
    ParseFn prefixRule = getRule(previous.type).prefix;
    if (prefixRule == nullptr) {
        error("Expect expression.");
        return;
    }

    (this->*prefixRule)();

    while (precedence <= getRule(current.type).precedence) {
        advance();
        ParseFn infixRule = getRule(previous.type).infix;
        (this->*infixRule)();
    }
}

Parser::ParseRule Parser::getRule(TokenType type) {
    // Prefix             Infix              Infix Precedence
    static ParseRule rules[TK_EOF + 1] = {
        [TK_LPARAN] = {&Parser::grouping, &Parser::call, PREC_CALL},
        [TK_RPARAN] = {nullptr, nullptr, PREC_NONE},
        [TK_LBRACE] = {nullptr, nullptr, PREC_NONE},
        [TK_RBRACE] = {nullptr, nullptr, PREC_NONE},
        [TK_COMMA] = {nullptr, nullptr, PREC_NONE},
        [TK_DOT] = {nullptr, nullptr, PREC_NONE},
        [TK_MINUS] = {&Parser::unary, &Parser::binary, PREC_TERM},
        [TK_PLUS] = {nullptr, &Parser::binary, PREC_TERM},
        [TK_SCOLON] = {nullptr, nullptr, PREC_NONE},
        [TK_SLASH] = {nullptr, &Parser::binary, PREC_FACTOR},
        [TK_STAR] = {nullptr, &Parser::binary, PREC_FACTOR},
        [TK_NOT] = {&Parser::unary, nullptr, PREC_NONE},
        [TK_NOTEQ] = {nullptr, &Parser::binary, PREC_EQUALITY},
        [TK_EQ] = {nullptr, nullptr, PREC_NONE},
        [TK_EQEQ] = {nullptr, &Parser::binary, PREC_EQUALITY},
        [TK_GT] = {nullptr, &Parser::binary, PREC_COMPARISON},
        [TK_GTEQ] = {nullptr, &Parser::binary, PREC_COMPARISON},
        [TK_LT] = {nullptr, &Parser::binary, PREC_COMPARISON},
        [TK_LTEQ] = {nullptr, &Parser::binary, PREC_COMPARISON},
        [TK_NAME] = {&Parser::variable, nullptr, PREC_NONE},
        [TK_STRING] = {&Parser::string, nullptr, PREC_NONE},
        [TK_NUMBER] = {&Parser::number, nullptr, PREC_NONE},
        [TK_AND] = {nullptr, nullptr, PREC_NONE},
        [TK_ELSE] = {nullptr, nullptr, PREC_NONE},
        [TK_FALSE] = {&Parser::literal, nullptr, PREC_NONE},
        [TK_FOR] = {nullptr, nullptr, PREC_NONE},
        [TK_FUNCTION] = {nullptr, nullptr, PREC_NONE},
        [TK_IF] = {nullptr, nullptr, PREC_NONE},
        [TK_NULL] = {&Parser::literal, nullptr, PREC_NONE},
        [TK_OR] = {nullptr, nullptr, PREC_NONE},
        [TK_RETURN] = {nullptr, nullptr, PREC_NONE},
        [TK_TRUE] = {&Parser::literal, nullptr, PREC_NONE},
        [TK_LET] = {nullptr, nullptr, PREC_NONE},
        [TK_WHILE] = {nullptr, nullptr, PREC_NONE},
        [TK_ERROR] = {nullptr, nullptr, PREC_NONE},
        [TK_EOF] = {nullptr, nullptr, PREC_NONE},
    };
    return rules[type];
}

void Parser::expression() { parsePrecedence(PREC_ASSIGNMENT); }

void Parser::varDeclaration() {
    uint32_t global = parseVariable("Expect variable name.");

    if (match(TK_EQ)) {
        expression();
    } else {
        emitByte(OP_NULL);
    }
    consume(TK_SCOLON, "Expect ';' after variable declaration.");

    defineVariable(global);
}

void Parser::blockBody() {
    while (!check(TK_RBRACE) && !check(TK_EOF)) {
        declaration();
    }

    consume(TK_RBRACE, "Expect '}' after block.");
}

void Parser::parseReturn() {
    if (check(TK_SCOLON)) {
        emitReturn();
        advance();
    } else {
        expression();
        consume(TK_SCOLON, "Expect ';' after return value.");
        emitByte(OP_RETURN);
    }
}

void Parser::functionDeclaration() {
    const uint32_t global = parseVariable("Expect function name.");
    if (bytecode->name.back() == "main") {
        isMainThere = 1;
    }
    emitBytes(OP_BEG_FUNC, global);
    function();
    emitByte(OP_END_FUNC);
}

void Parser::function() {
    consume(TK_LPARAN, "Expect '(' after function name.");
    while (!check(TK_RPARAN))
    {
        auto name = parseVariable("A name is expected.");
        emitBytes(OP_DEF_PARAM, name);
        if (!(check(TK_RPARAN)))
        {
            consume(TK_COMMA, "Expect ',' after function name.");
        }
    }

    consume(TK_RPARAN, "Expect ')' after parameter.");
    consume(TK_LBRACE, "Expect '{' before function body.");
    blockBody();

    // if blockBody contains return statement skip OP_NULL
    if (bytecode->opcode.empty() || bytecode->opcode.back() != OP_RETURN) {
        emitByte(OP_NULL);
    }
}

void Parser::expressionStatement() {
    expression();
    consume(TK_SCOLON, "Expect ';' after expression.");
    emitByte(OP_POP);
}

void Parser::ifStatement() {
    consume(TK_LPARAN, "Expect '(' after 'if'.");
    expression();
    consume(TK_RPARAN, "Expect ')' after condition.");

    const int thenJump = emitJump(OP_JUMP_IF_NOT);
    statement();

    const int elseJump = emitJump(OP_JUMP);

    patchJump(thenJump);

    if (match(TK_ELSE)) {
        statement();
    }

    patchJump(elseJump);
}

void Parser::declaration() {
    if (match(TK_LET)) {
        varDeclaration();
    } else if (match(TK_FUNCTION)) {
        functionDeclaration();
    } else {
        statement();
    }
}

void Parser::statement() {
    if (match(TK_RETURN)) {
        parseReturn();
    } else if (match(TK_IF)) {
        ifStatement();
    } else if (match(TK_LBRACE)) {
        blockBody();
    } else {
        expressionStatement();
    }
}

bool Parser::compile(Bytecode *bcode) {
    bytecode = bcode;
    advance();
    while (!match(TK_EOF)) {
        declaration();
    }
    endCompiler();

    return !hadError; // return true on success
}
