#ifndef lang_generator_h
#define lang_generator_h

#include "public.h"
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

    InterpretResult run();
    void runtimeError(const std::string error);
    void push(const std::string &reg);
    void pop(const std::string &reg);

    Value* value;
    int stackSize;

    std::stringstream assembly_main;
    std::stringstream assembly_text;
    std::stringstream assembly_data;
    std::vector<uint8_t> opcode;
public:
    Generator();
    void free();
    InterpretResult main(const std::string source);

};

extern Generator generator;
#endif