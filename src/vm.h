#ifndef lang_vm_h
#define lang_vm_h

#include "chunk.h"
#include "value.h"
#include "common.h"

#define STACK_MAX 256

typedef enum {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} InterpretResult;

class VM {
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

    std::stringstream assembly;
    uint8_t* ip;
public:
    VM();
    void free();
    InterpretResult run(const std::string source);

};

extern VM vm;
#endif