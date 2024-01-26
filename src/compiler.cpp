#include <stdio.h>

#include "common.h"
#include "compiler.h"
#include "scanner.h"

typedef struct {
  Token current;
  Token previous;
  bool hadError;
  bool panicMode;
} Parser;

typedef enum {
  PREC_NONE,
  PREC_ASSIGNMENT,  // =
  PREC_OR,          // or
  PREC_AND,         // and
  PREC_EQUALITY,    // == !=
  PREC_COMPARISON,  // < > <= >=
  PREC_TERM,        // + -
  PREC_FACTOR,      // * /
  PREC_UNARY,       // ! -
  PREC_CALL,        // . ()
  PREC_PRIMARY
} Precedence;

typedef void (*ParseFn)();

typedef struct {
  ParseFn prefix;
  ParseFn infix;
  Precedence precedence;
} ParseRule;

Parser parser;
Scanner scanner;
Value* compilingValue;

static Value* currentValue() {
  return compilingValue;
}

static void errorAt(Token* token, const std::string message) {
  if (parser.panicMode) return;
  parser.panicMode = true;
  cerr << "[line " << token->line << "] Error";

  if (token->type == TK_EOF) {
    cerr << " at end";
  } else if (token->type == TK_ERROR) {
    // Nothing.
  } else {
    fprintf(stderr, " at '%.*s'", token->length, token->start);
  }

  cerr << ": " << message << endl;
  parser.hadError = true;
}

static void error(const std::string message) {
  errorAt(&parser.previous, message);
}

static void errorAtCurrent(const std::string message) {
  errorAt(&parser.current, message);
}

static void advance() {
  parser.previous = parser.current;

  for (;;) {
    parser.current = scanner.scanToken();
    if (parser.current.type != TK_ERROR) break;

    errorAtCurrent(parser.current.start);
  }
}

static bool check(TokenType type) {
  return parser.current.type == type;
}

static void consume(TokenType type, const std::string message) {
  if (check(type)) {
    advance();
    return;
  }

  errorAtCurrent(message);
}

static bool match(TokenType type) {
  if (!check(type)) return false;
  advance();
  return true;
}

static void emitByte(uint8_t byte) {
  currentValue()->write(byte, parser.previous.line);
}

static void emitBytes(uint8_t byte1, uint8_t byte2) {
  emitByte(byte1);
  emitByte(byte2);
}

static void emitReturn() {
  emitByte(OP_RETURN);
}

static uint8_t makeConstant(int value) {
  int constant = currentValue()->addValue(value);
  if (constant > UINT8_MAX) {
    error("Too many constants in one chunk.");
    return 0;
  }

  return (uint8_t)constant;
}

static void emitConstant(int value) {
  emitBytes(OP_CONSTANT, makeConstant(value));
}

static void endCompiler() {
  emitReturn();
  #ifdef DEBUG_PRINT_CODE
    if (!parser.hadError) {
      Debug debug(*currentValue());
      debug.disassemble("code");
    }
  #endif
}

static void expression();
static void statement();
static void declaration();
static ParseRule* getRule(TokenType type);
static void parsePrecedence(Precedence precedence);

static void binary() {
  TokenType operatorType = parser.previous.type;
  ParseRule* rule = getRule(operatorType);
  parsePrecedence((Precedence)(rule->precedence + 1));

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

static void literal() {
  switch (parser.previous.type) {
    case TK_FALSE: emitByte(OP_FALSE); break;
    case TK_NULL: emitByte(OP_NIL); break;
    case TK_TRUE: emitByte(OP_TRUE); break;
    default: return; // Unreachable.
  }
}

static void grouping() {
  expression();
  consume(TK_RIGHT_PAREN, "Expect ')' after expression.");
}

static void number() {
  double value = strtod(parser.previous.start, NULL);
  emitConstant((value));
}

static void unary() {
  TokenType operatorType = parser.previous.type;

  // Compile the operand.
  parsePrecedence(PREC_UNARY);

  // Emit the operator instruction.
  switch (operatorType) {
    case TK_BANG: emitByte(OP_NOT); break;
    case TK_MINUS: emitByte(OP_NEGATE); break;
    default: return; // Unreachable.
  }
}

ParseRule rules[] = {
  [TK_LEFT_PAREN]    = {grouping, NULL,   PREC_NONE},
  [TK_RIGHT_PAREN]   = {NULL,     NULL,   PREC_NONE},
  [TK_LEFT_BRACE]    = {NULL,     NULL,   PREC_NONE},
  [TK_RIGHT_BRACE]   = {NULL,     NULL,   PREC_NONE},
  [TK_COMMA]         = {NULL,     NULL,   PREC_NONE},
  [TK_DOT]           = {NULL,     NULL,   PREC_NONE},
  [TK_MINUS]         = {unary,    binary, PREC_TERM},
  [TK_PLUS]          = {NULL,     binary, PREC_TERM},
  [TK_SEMICOLON]     = {NULL,     NULL,   PREC_NONE},
  [TK_SLASH]         = {NULL,     binary, PREC_FACTOR},
  [TK_STAR]          = {NULL,     binary, PREC_FACTOR},
  [TK_BANG]          = {unary,    NULL,   PREC_NONE},
  [TK_BANG_EQUAL]    = {NULL,     binary, PREC_EQUALITY},
  [TK_EQUAL]         = {NULL,     NULL,   PREC_NONE},
  [TK_EQUAL_EQUAL]   = {NULL,     binary, PREC_EQUALITY},
  [TK_GREATER]       = {NULL,     binary, PREC_COMPARISON},
  [TK_GREATER_EQUAL] = {NULL,     binary, PREC_COMPARISON},
  [TK_LESS]          = {NULL,     binary, PREC_COMPARISON},
  [TK_LESS_EQUAL]    = {NULL,     binary, PREC_COMPARISON},
  [TK_IDENTIFIER]    = {NULL,     NULL,   PREC_NONE},
  [TK_STRING]        = {NULL,     NULL,   PREC_NONE},
  [TK_NUMBER]        = {number,   NULL,   PREC_NONE},
  [TK_AND]           = {NULL,     NULL,   PREC_NONE},
  [TK_ELSE]          = {NULL,     NULL,   PREC_NONE},
  [TK_FALSE]         = {literal,  NULL,   PREC_NONE},
  [TK_FOR]           = {NULL,     NULL,   PREC_NONE},
  [TK_FUNCTION]      = {NULL,     NULL,   PREC_NONE},
  [TK_IF]            = {NULL,     NULL,   PREC_NONE},
  [TK_NULL]          = {literal,  NULL,   PREC_NONE},
  [TK_OR]            = {NULL,     NULL,   PREC_NONE},
  [TK_PRINT]         = {NULL,     NULL,   PREC_NONE},
  [TK_RETURN]        = {NULL,     NULL,   PREC_NONE},
  [TK_TRUE]          = {literal,  NULL,   PREC_NONE},
  [TK_VAR]           = {NULL,     NULL,   PREC_NONE},
  [TK_WHILE]         = {NULL,     NULL,   PREC_NONE},
  [TK_ERROR]         = {NULL,     NULL,   PREC_NONE},
  [TK_EOF]           = {NULL,     NULL,   PREC_NONE},
};

static void parsePrecedence(Precedence precedence) {
  advance();
  ParseFn prefixRule = getRule(parser.previous.type)->prefix;
  if (prefixRule == NULL) {
    error("Expect expression.");
    return;
  }

  prefixRule();

  while (precedence <= getRule(parser.current.type)->precedence) {
    advance();
    ParseFn infixRule = getRule(parser.previous.type)->infix;
    infixRule();
  }
}

static ParseRule* getRule(TokenType type) {
  return &rules[type];
}

static void expression() {
   parsePrecedence(PREC_ASSIGNMENT);
}

static void printStatement() {
  expression();
  consume(TK_SEMICOLON, "Expect ';' after value.");
  emitByte(OP_PRINT);
}

static void declaration() {
  statement();
}

static void statement() {
  if (match(TK_PRINT)) {
    printStatement();
  }
}

bool compile(const std::string source, Value* value) {
  scanner.put(source.c_str());
  compilingValue = value;

  parser.hadError = false;
  parser.panicMode = false;

  advance();
  while (!match(TK_EOF)) {
    declaration();
  }
  endCompiler();
  return !parser.hadError;
}
