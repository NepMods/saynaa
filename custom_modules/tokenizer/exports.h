/*
 * Copyright (c) 2023-2024 Mohammed Abdifatah. All rights reserved.
 * Distributed Under The MIT License
 *
 */

 #ifndef saynaa_scanner_h
 #define saynaa_scanner_h
 
 #include "../../shared/common.h"
 
 typedef enum {
   // Single-character tokens.
   TK_LPARAN, // (
   TK_RPARAN, // )
   TK_LBRACE, // {
   TK_RBRACE, // }
   TK_COMMA,  // ,
   TK_DOT,    // .
   TK_MINUS,  // -
   TK_PLUS,   // +
   TK_SCOLON, // ;
   TK_SLASH,  // /
   TK_STAR,   // *
 
   // One or two character tokens.
   TK_NOT,   // !
   TK_NOTEQ, // !=
   TK_EQ,    // =
   TK_EQEQ,  // ==
   TK_GT,    // >
   TK_GTEQ,  // >=
   TK_LT,    // <
   TK_LTEQ,  // <=
 
   // Literals.
   TK_NAME,   // NAME
   TK_STRING, // ""
   TK_NUMBER, // 0-9
 
   // Keywords.
   TK_AND,      // and
   TK_ELSE,     // else
   TK_FALSE,    // false
   TK_FOR,      // for
   TK_FUNCTION, // function
   TK_IF,       // if
   TK_NULL,     // null
   TK_OR,       // or
   TK_PRINT,    // print
   TK_RETURN,   // return
   TK_TRUE,     // true
   TK_LET,      // let
   TK_WHILE,    // while
 
   TK_ERROR, //
   TK_EOF    // \0
 } TokenType;
 
 typedef struct {
   TokenType type;
   const char *start;
   int length;
   int line;
 } Token;
 
 typedef struct {
   const char *identifier;
   int length;
   TokenType tk_type;
 } _Keyword;
 
 // List of keywords mapped into their identifiers.
 static _Keyword _keywords[] = {
     {"print", 5, TK_PRINT},    {"and", 3, TK_AND},
     {"else", 4, TK_ELSE},      {"false", 5, TK_FALSE},
     {"for", 3, TK_FOR},        {"function", 8, TK_FUNCTION},
     {"if", 2, TK_IF},          {"null", 4, TK_NULL},
     {"or", 2, TK_OR},          {"return", 6, TK_RETURN},
     {"true", 4, TK_TRUE},      {"let", 3, TK_LET},
     {"while", 5, TK_WHILE},
 
     {NULL, 0, (TokenType)(0)}, // Sentinel to mark the end of the array.
 };
 
 class Scanner {
   bool isAtEnd();
   Token makeToken(TokenType type);
   Token errorToken(const char *message);
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
   TokenType checkKeyword(int start, int length, const char *rest,
                          TokenType type);
 
 
 public:

 const char *start;
 const char *current;
 int line = 1;
   void put(const std::string &source);
   Token scanToken();
 };
 
 #endif