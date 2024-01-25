#ifndef lang_chunk_h
#define lang_chunk_h

#include "common.h"
#include "value.h"

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

class Chunk {
private:
    int disassembleInstruction(int offset);
    int simpleInstruction(const std::string name, int offset);
    int constantInstruction(const std::string name, int offset);

    int count;
    int capacity;
    int* lines;
public:
    Chunk();
    int addConstant(Value value);
    void write(uint8_t byte, int line);
    void disassemble(const std::string name);
    void free();
    ValueArray constants;

    uint8_t* code;
};

#endif