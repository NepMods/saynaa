#ifndef lang_generator_h
#define lang_generator_h

#include "chunk.h"
#include "value.h"
#include "common.h"

#define STACK_MAX 256

typedef enum {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} InterpretResult;

class Generator {
private:
    enum {
      V_ADD,
      V_SUB,
      V_MUL,
      V_DIV,
      V_NOTHING,
    };

    InterpretResult bytecode();
    void runtimeError(const std::string error);
    void push(const std::string &reg);
    void pop(const std::string &reg);
    int checkAndReturnValue(Value val);

    Chunk* chunk;
    Value* stackSize;

    std::stringstream assembly_main;
    std::stringstream assembly_text;
    std::stringstream assembly_data;
    uint8_t* ip;
public:
    Generator();
    void free();
    InterpretResult run(const std::string source);

};

extern Generator generator;
#endif