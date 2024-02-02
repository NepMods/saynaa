#include "compiler.h"

void Parser::errorAt(Token* token, const std::string message) {
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

void Parser::error(const std::string message) {
  errorAt(&previous, message);
}

void Parser::errorAtCurrent(const std::string message) {
  errorAt(&current, message);
}

void Parser::advance() {
  previous = current;

  for (;;) {
    current = scanner.scanToken();
    if (current.type != TK_ERROR) break;

    errorAtCurrent(current.start);
  }
}

bool Parser::check(TokenType type) {
  return current.type == type;
}

void Parser::consume(TokenType type, const std::string message) {
  if (check(type)) {
    advance();
    return;
  }

  errorAtCurrent(message);
}

bool Parser::match(TokenType type) {
  if (!check(type)) return false;
  advance();
  return true;
}

void Parser::emitByte(uint8_t byte) {
  ParserValue->opcode.push_back(byte);
  ParserValue->lines.push_back(previous.line);
}

void Parser::emitBytes(uint8_t byte1, uint8_t byte2) {
  emitByte(byte1);
  emitByte(byte2);
}

void Parser::emitReturn() {
  emitByte(OP_RETURN);
}

uint8_t Parser::makeConstant(int value) {
  ParserValue->value.push_back(value);
  int constant = ParserValue->value.size() -1;

  if (constant > UINT8_MAX) {
    error("Too many constants in one chunk.");
    return 0;
  }

  return (uint8_t)constant;
}

void Parser::emitConstant(int value) {
  emitBytes(OP_CONSTANT, makeConstant(value));
}

void Parser::endCompiler() {
  emitReturn();
  #ifdef DEBUG_PRINT_CODE
    if (!hadError) {
      Debug debug(*ParserValue);
      debug.disassemble("OPCODE");
    }
  #endif
}

uint8_t Parser::identifierConstant(Token* name) {
  ParserValue->value.push_back(copyString(name->start, name->length));
  int constant = ParserValue->value.size() -1;

  if (constant > UINT8_MAX) {
    error("Too many constants in one chunk.");
    return 0;
  }

  return (uint8_t)constant;
}

uint8_t Parser::parseVariable(const char* errorMessage) {
  consume(TK_NAME, errorMessage);
  return identifierConstant(&previous);
}

void Parser::defineVariable(uint8_t local) {
  emitBytes(OP_DEFINE_LOCAL, local);
}

void Parser::binary() {
  TokenType operatorType = previous.type;
  ParseRule rule = getRule(operatorType);
  parsePrecedence((Precedence)(rule.precedence + 1));

  switch (operatorType) {
    case TK_BANG_EQUAL:    emitBytes(OP_EQUAL, OP_NOT); break;
    case TK_EQUAL_EQUAL:   emitByte(OP_EQUAL); break;
    case TK_GREATER:       emitByte(OP_GREATER); break;
    case TK_GREATER_EQUAL: emitBytes(OP_LESS, OP_NOT); break;
    case TK_LESS:          emitByte(OP_LESS); break;
    case TK_LESS_EQUAL:    emitBytes(OP_GREATER, OP_NOT); break;
    case TK_PLUS:          emitByte(OP_ADD); break;
    case TK_MINUS:         emitByte(OP_SUBTRACT); break;
    case TK_STAR:          emitByte(OP_MULTIPLY); break;
    case TK_SLASH:         emitByte(OP_DIVIDE); break;
    default: return; // Unreachable.
  }
}

void Parser::literal() {
  switch (previous.type) {
    case TK_FALSE: emitByte(OP_FALSE); break;
    case TK_NULL: emitByte(OP_NIL); break;
    case TK_TRUE: emitByte(OP_TRUE); break;
    default: return; // Unreachable.
  }
}

void Parser::grouping() {
  expression();
  consume(TK_RIGHT_PAREN, "Expect ')' after expression.");
}

void Parser::number() {
  double value = strtod(previous.start, nullptr);
  emitConstant((value));
}

void Parser::namedVariable(Token name) {
  uint8_t arg = identifierConstant(&name);
  emitBytes(OP_GET_LOCAL, arg);
}

void Parser::variable() {
  namedVariable(previous);
}

void Parser::unary() {
  TokenType operatorType = previous.type;

  // Compile the operand.
  parsePrecedence(PREC_UNARY);

  // Emit the operator instruction.
  switch (operatorType) {
    case TK_BANG: emitByte(OP_NOT); break;
    case TK_MINUS: emitByte(OP_NEGATE); break;
    default: return; // Unreachable.
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
                      // Prefix               Infix              Infix Precedence
  #define NO_RULE        {nullptr,            nullptr,           PREC_NONE}
  ParseRule rules[TK_EOF+1] = {
    [TK_LEFT_PAREN]    = {&Parser::grouping,  nullptr,           PREC_NONE},
    [TK_RIGHT_PAREN]   = NO_RULE,
    [TK_LEFT_BRACE]    = NO_RULE,
    [TK_RIGHT_BRACE]   = NO_RULE,
    [TK_COMMA]         = NO_RULE,
    [TK_DOT]           = NO_RULE,
    [TK_MINUS]         = {&Parser::unary,     &Parser::binary,   PREC_TERM},
    [TK_PLUS]          = {nullptr,            &Parser::binary,   PREC_TERM},
    [TK_SEMICOLON]     = NO_RULE,
    [TK_SLASH]         = {nullptr,            &Parser::binary,   PREC_FACTOR},
    [TK_STAR]          = {nullptr,            &Parser::binary,   PREC_FACTOR},
    [TK_BANG]          = {&Parser::unary,     nullptr,           PREC_NONE},
    [TK_BANG_EQUAL]    = {nullptr,            &Parser::binary,   PREC_EQUALITY},
    [TK_EQUAL]         = NO_RULE,
    [TK_EQUAL_EQUAL]   = {nullptr,            &Parser::binary,   PREC_EQUALITY},
    [TK_GREATER]       = {nullptr,            &Parser::binary,   PREC_COMPARISON},
    [TK_GREATER_EQUAL] = {nullptr,            &Parser::binary,   PREC_COMPARISON},
    [TK_LESS]          = {nullptr,            &Parser::binary,   PREC_COMPARISON},
    [TK_LESS_EQUAL]    = {nullptr,            &Parser::binary,   PREC_COMPARISON},
    [TK_NAME]          = {&Parser::variable,  nullptr,           PREC_NONE},
    [TK_STRING]        = NO_RULE,
    [TK_NUMBER]        = {&Parser::number,    nullptr,           PREC_NONE},
    [TK_AND]           = NO_RULE,
    [TK_ELSE]          = NO_RULE,
    [TK_FALSE]         = {&Parser::literal,   nullptr,           PREC_NONE},
    [TK_FOR]           = NO_RULE,
    [TK_FUNCTION]      = NO_RULE,
    [TK_IF]            = NO_RULE,
    [TK_NULL]          = {&Parser::literal,   nullptr,           PREC_NONE},
    [TK_OR]            = NO_RULE,
    [TK_PRINT]         = NO_RULE,
    [TK_RETURN]        = NO_RULE,
    [TK_TRUE]          = {&Parser::literal,   nullptr,           PREC_NONE},
    [TK_LET]           = NO_RULE,
    [TK_WHILE]         = NO_RULE,
    [TK_ERROR]         = NO_RULE,
    [TK_EOF]           = NO_RULE,
  };
  return rules[type];
}

void Parser::expression() {
   parsePrecedence(PREC_ASSIGNMENT);
}

void Parser::varDeclaration() {
  uint8_t global = parseVariable("Expect variable name.");

  if (match(TK_EQUAL)) {
    expression();
  } else {
    emitByte(OP_NIL);
  }
  consume(TK_SEMICOLON,
          "Expect ';' after variable declaration.");

  defineVariable(global);
}

void Parser::expressionStatement() {
  expression();
  consume(TK_SEMICOLON, "Expect ';' after expression.");
  emitByte(OP_POP);
}

void Parser::printStatement() {
  expression();
  consume(TK_SEMICOLON, "Expect ';' after value.");
  emitByte(OP_PRINT);
}

void Parser::declaration() {
  if (match(TK_LET)) {
    varDeclaration();
  } else {
    statement();
  }
}

void Parser::statement() {
  if (match(TK_PRINT)) {
    printStatement();
  } else {
    expressionStatement();
  }
}

bool Parser::compile(const std::string source, Value* value) {
  scanner.put(source.c_str());
  ParserValue = value;

  hadError = false;

  advance();
  while (!match(TK_EOF)) {
    declaration();
  }
  endCompiler();
  return !hadError;
}
