//
// Created by arjun on 7/21/25.
//

#ifndef PARSER_H
#define PARSER_H
#include <cstdio>
#include <iostream>
#include <string>
#include "../exports.h"

class Parser {
    Token current;
    Token previous;
    bool hadError;
    Bytecode *bytecode;
    int index = 0;
    Token *all_tokens;


    typedef void (Parser::*ParseFn)();
    typedef struct {
        ParseFn prefix;
        ParseFn infix;
        Precedence precedence;
    } ParseRule;

    void errorAt(Token *token, const std::string message);
    void error(const std::string message);
    void errorAtCurrent(const std::string message);
    void advance();
    bool check(TokenType type);
    void consume(TokenType type, const std::string message);
    bool match(TokenType type);
    void emitByte(uint32_t byte);
    void emitBytes(uint32_t byte1, uint32_t byte2);
    void emitReturn();
    uint32_t makeConstant(std::variant<int, std::string> value);
    int emitJump(uint8_t instruction);
    void patchJump(int offset, int value = 0);
    void emitConstant(std::variant<int, std::string> value);
    void endCompiler();
    uint32_t identifierConstant(Token *name);
    uint32_t parseVariable(const char *errorMessage);
    void defineVariable(uint32_t global);
    void binary();
    void call();
    void literal();
    void grouping();
    void number();
    void string();
    void namedVariable(Token name);
    void variable();
    void unary();
    void parsePrecedence(Precedence precedence);
    ParseRule getRule(TokenType type);
    void expression();
    void varDeclaration();
    void blockBody();
    void parseReturn();
    void functionDeclaration();
    void function();
    void expressionStatement();
    void ifStatement();
    void declaration();
    void statement();
    int isMainThere = 0;

public:
    Parser(Token *tk_list);
    bool compile(Bytecode *bcode);
};



#endif //PARSER_H
