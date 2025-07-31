//
// Created by arjun on 7/20/25.
//

#ifndef TOKENIZER_H
#define TOKENIZER_H
#include <string>
#include "../exports.h"


class Tokenizer {
    const char *start;
    const char *current;
    int line = 1;
    bool isAtEnd();
    Token makeToken(TokenType type);
    Token errorToken(const char *message);
    char advance();
    bool match(char expected);
    void skipWhitespace();
    char peek();
    char peekNext();
    Token string();
    Token multi_string();
    bool isDigit(char c);
    Token number();
    bool isAlpha(char c);
    Token identifier();
    Token scanToken();

public:

    Tokenizer(std::string &source);
    void scan(Token **ret_token);
};



#endif //TOKENIZER_H
