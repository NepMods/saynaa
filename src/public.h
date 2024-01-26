#ifndef lang_public_h
#define lang_public_h

#include "common.h"

typedef enum {
   OP_CONSTANT,
   OP_NIL,
   OP_TRUE,
   OP_FALSE,
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

class Value {
private:
public:
    Value();
    int addValue(int value);
    void write(uint8_t opcode, int line);
    void free();

    std::vector<int> value;
    std::vector<int> lines;
    std::vector<uint8_t> opcode;
};

#endif
