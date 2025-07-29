//
// Created by arjun on 7/20/25.
//

#ifndef EXPORTS_H
#define EXPORTS_H
#include <cstdint>
#include <variant>
#include <vector>

#include "utils/utils.h"
#include "utils/debug.h"
enum TokenType
{
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
    TK_RETURN,   // return
    TK_TRUE,     // true
    TK_LET,      // let
    TK_WHILE,    // while

    TK_EXPORTED,
    TK_IMPORT,
    TK_IMPORT_FROM,

    TK_ERROR, //
    TK_EOF    // \0
};

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


static _Keyword _keywords[] = {
    {"and", 3, TK_AND},
    {"else", 4, TK_ELSE},      {"false", 5, TK_FALSE},
    {"for", 3, TK_FOR},        {"function", 8, TK_FUNCTION},
    {"if", 2, TK_IF},          {"null", 4, TK_NULL},
    {"or", 2, TK_OR},          {"return", 6, TK_RETURN},
    {"true", 4, TK_TRUE},      {"let", 3, TK_LET},
    {"while", 5, TK_WHILE},    {"import", 6, TK_IMPORT},
    {"from", 4, TK_IMPORT_FROM},   {"export", 6, TK_EXPORTED},

    {NULL, 0, (TokenType)(0)}, // Sentinel to mark the end of the array.
};

typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT, // =
    PREC_OR,         // or
    PREC_AND,        // and
    PREC_EQUALITY,   // == !=
    PREC_COMPARISON, // < > <= >=
    PREC_TERM,       // + -
    PREC_FACTOR,     // * /
    PREC_UNARY,      // ! -
    PREC_CALL,       // . ()
    PREC_PRIMARY
  } Precedence;

std::string opcodeToName(uint32_t opcode);
#endif //EXPORTS_H
