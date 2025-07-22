//
// Created by arjun on 7/20/25.
//

#include "Tokenizer.h"

#include <cstring>

Tokenizer::Tokenizer(std::string& source)
{
  start = source.c_str();
  current = start;
}

void Tokenizer::scan(Token** ret_token)
{
  Token* token_list = nullptr;
  size_t count = 0;

  Token current_token = scanToken();
  while (current_token.type != TK_EOF)
  {
    token_list = (Token*)realloc(token_list, (count + 1) * sizeof(Token));
    token_list[count++] = current_token;
    current_token = scanToken();
  }
  token_list = (Token*)realloc(token_list, (count + 1) * sizeof(Token));
  token_list[count++] = current_token;

  *ret_token = token_list;
}


Token Tokenizer::scanToken() {
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
    case '`':
      return multi_string();
    case '"':
      return string();
    }
  
    std::string error_message = "Unexpected character: '";
     error_message += c;
    return errorToken(error_message.c_str());
  }
  
  Token Tokenizer::string() {
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
  Token Tokenizer::multi_string() {
    while (peek() != '`' && !isAtEnd()) {
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

  bool Tokenizer::isAtEnd() { return *current == '\0'; }
  
  Token Tokenizer::number() {
    while (isDigit(peek()))
      advance();
  
    // Look for a fractional part.
    if (peek() == '.' && isDigit(peekNext())) {
      advance();
  
      while (isDigit(peek()))
        advance();
    }
  
    return makeToken(TK_NUMBER);
  }
  
  Token Tokenizer::identifier() {
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
  
  bool Tokenizer::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
  }
  
  bool Tokenizer::isDigit(char c) { return c >= '0' && c <= '9'; }
  
  char Tokenizer::advance() {
    current++;
    return current[-1];
  }
  
  char Tokenizer::peek() { return *current; }
  
  void Tokenizer::skipWhitespace() {
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
  
  char Tokenizer::peekNext() {
    if (isAtEnd())
      return '\0';
    return current[1];
  }
  
  bool Tokenizer::match(char expected) {
    if (isAtEnd())
      return false;
    if (*current != expected)
      return false;
    current++;
    return true;
  }
  
  Token Tokenizer::makeToken(TokenType type) {
    Token token;
    token.type = type;
    token.start = start;
    token.length = (int)(current - start);
    token.line = line;
    return token;
  }
  
  Token Tokenizer::errorToken(const char *message) {
    Token token;
    token.type = TK_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = line;
    return token;
  }
 
 