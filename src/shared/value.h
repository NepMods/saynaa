#ifndef lang_public_h
#define lang_public_h

#include "common.h"

typedef enum {
   OP_CONSTANT,
   OP_NIL,
   OP_TRUE,
   OP_FALSE,
   OP_POP,
   OP_DEFINE_LOCAL,
   OP_GET_LOCAL,
   OP_EQUAL,
   OP_GREATER,
   OP_LESS,
   OP_ADD,
   OP_SUBTRACT,
   OP_MULTIPLY,
   OP_DIVIDE,
   OP_NOT,
   OP_NEGATE,
   OP_PRINT,
   OP_RETURN,
} OpCode;

typedef struct {
    std::vector<std::variant<int, std::string>> value;
    std::vector<int> lines;
    std::vector<uint8_t> opcode;

    void free() {
        value.clear();
        lines.clear();
        opcode.clear();
    }
} Value;

typedef struct {
    std::string name;       // variable name
    int stackLocation;      // stack location in assembly
} Variable;

#endif
