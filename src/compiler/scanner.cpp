/*
 * Copyright (c) 2023-2024 Mohammed Abdifatah. All rights reserved.
 * Distributed Under The MIT License
 *
 */

#include "scanner.h"

void Scanner::put(const std::string &source) {
  start = source.c_str();
  current = source.c_str();
}

Token Scanner::scanToken() {
  skipWhitespace();
  start = current;

  if (isAtEnd())
    return makeToken(TK_EOF);

  char c = advance();
  if (isAlpha(c))
    return identifier();
  if (isDigit(c))
    return number();

  switch (c) {
  case '(':
    return makeToken(TK_LPARAN);
  case ')':
    return makeToken(TK_RPARAN);
  case '{':
    return makeToken(TK_LBRACE);
  case '}':
    return makeToken(TK_RBRACE);
  case ';':
    return makeToken(TK_SCOLON);
  case ',':
    return makeToken(TK_COMMA);
  case '.':
    return makeToken(TK_DOT);
  case '-':
    return makeToken(TK_MINUS);
  case '+':
    return makeToken(TK_PLUS);
  case '/':
    return makeToken(TK_SLASH);
  case '*':
    return makeToken(TK_STAR);
  case '!':
    return makeToken(match('=') ? TK_NOTEQ : TK_NOT);
  case '=':
    return makeToken(match('=') ? TK_EQEQ : TK_EQ);
  case '<':
    return makeToken(match('=') ? TK_LTEQ : TK_LT);
  case '>':
    return makeToken(match('=') ? TK_GTEQ : TK_GT);
  case '"':
    return string();
  }

  return errorToken("Unexpected character.");
}

Token Scanner::string() {
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n')
      line++;
    advance();
  }

  if (isAtEnd())
    return errorToken("Unterminated string.");

  // The closing quote.
  advance();
  return makeToken(TK_STRING);
}

bool Scanner::isAtEnd() { return *current == '\0'; }

Token Scanner::number() {
  while (isDigit(peek()))
    advance();

  // Look for a fractional part.
  if (peek() == '.' && isDigit(peekNext())) {
    // Consume the ".".
    advance();

    while (isDigit(peek()))
      advance();
  }

  return makeToken(TK_NUMBER);
}

Token Scanner::identifier() {
  while (isAlpha(peek()) || isDigit(peek()))
    advance();

  const char *name_start = start;
  TokenType type = TK_NAME;

  int length = (int)(current - name_start);
  for (int i = 0; _keywords[i].identifier != NULL; i++) {
    if (_keywords[i].length == length &&
        strncmp(name_start, _keywords[i].identifier, length) == 0) {
      type = _keywords[i].tk_type;
      break;
    }
  }

  return makeToken(type);
}

bool Scanner::isAlpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Scanner::isDigit(char c) { return c >= '0' && c <= '9'; }

char Scanner::advance() {
  current++;
  return current[-1];
}

char Scanner::peek() { return *current; }

void Scanner::skipWhitespace() {
  for (;;) {
    char c = peek();
    switch (c) {
    case ' ':
    case '\r':
    case '\t':
      advance();
      break;
    case '\n':
      line++;
      advance();
      break;
    case '/':
      if (peekNext() == '/') {
        // A comment goes until the end of the line.
        while (peek() != '\n' && !isAtEnd())
          advance();
      } else {
        return;
      }
      break;
    default:
      return;
    }
  }
}

char Scanner::peekNext() {
  if (isAtEnd())
    return '\0';
  return current[1];
}

bool Scanner::match(char expected) {
  if (isAtEnd())
    return false;
  if (*current != expected)
    return false;
  current++;
  return true;
}

Token Scanner::makeToken(TokenType type) {
  Token token;
  token.type = type;
  token.start = start;
  token.length = (int)(current - start);
  token.line = line;
  return token;
}

Token Scanner::errorToken(const char *message) {
  Token token;
  token.type = TK_ERROR;
  token.start = message;
  token.length = (int)strlen(message);
  token.line = line;
  return token;
}