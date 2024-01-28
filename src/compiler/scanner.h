#ifndef lang_scanner_h
#define lang_scanner_h

#include "../shared/common.h"

typedef enum {
  // Single-character tokens.
  TK_LEFT_PAREN,
  TK_RIGHT_PAREN,
  TK_LEFT_BRACE,
  TK_RIGHT_BRACE,
  TK_COMMA,
  TK_DOT,
  TK_MINUS,
  TK_PLUS,
  TK_SEMICOLON,
  TK_SLASH,
  TK_STAR,

  // One or two character tokens.
  TK_BANG,
  TK_BANG_EQUAL,
  TK_EQUAL,
  TK_EQUAL_EQUAL,
  TK_GREATER,
  TK_GREATER_EQUAL,
  TK_LESS,
  TK_LESS_EQUAL,

  // Literals.
  TK_IDENTIFIER,
  TK_STRING,
  TK_NUMBER,

  // Keywords.
  TK_AND,
  TK_ELSE,
  TK_FALSE,
  TK_FOR,
  TK_FUNCTION,
  TK_IF,
  TK_NULL,
  TK_OR,
  TK_PRINT,
  TK_RETURN,
  TK_TRUE,
  TK_VAR,
  TK_WHILE,

  TK_ERROR,
  TK_EOF
} TokenType;

typedef struct {
    TokenType type;
    const char* start;
    int length;
    int line;
} Token;

typedef struct {
  const char* identifier;
  int length;
  TokenType tk_type;
} _Keyword;

// List of keywords mapped into their identifiers.
static _Keyword _keywords[] =  {
  { "print",    5, TK_PRINT    },
  { "and",      3, TK_AND      },
  { "else",     4, TK_ELSE     },
  { "false",    5, TK_FALSE    },
  { "for",      3, TK_FOR      },
  { "function", 8, TK_FUNCTION },
  { "if",       2, TK_IF       },
  { "null",     4, TK_NULL     },
  { "or",       2, TK_OR       },
  { "return",   6, TK_RETURN   },
  { "true",     4, TK_TRUE     },
  { "var",      3, TK_VAR      },
  { "while",    5, TK_WHILE    },

  { NULL,   0, (TokenType)(0) }, // Sentinel to mark the end of the array.
};

class Scanner {
private:
    bool isAtEnd();
    Token makeToken(TokenType type);
    Token errorToken(const char* message);
    char advance();
    bool match(char expected);
    void skipWhitespace();
    char peek();
    char peekNext();
    Token string();
    bool isDigit(char c);
    Token number();
    bool isAlpha(char c);
    Token identifier();
    TokenType identifierType();
    TokenType checkKeyword(int start, int length, const char* rest, TokenType type);

    const char* start;
    const char* current;
    int line;
public:
    Scanner();
    void put(const char* source);
    Token scanToken();

};

#endif