#ifndef lang_generator_h
#define lang_generator_h

#include "../compiler/compiler.h"
#include "../shared/common.h"
#include "../shared/value.h"

typedef enum {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} InterpretResult;

class Generator {
  enum {
    V_ADD,
    V_SUB,
    V_MUL,
    V_DIV,
    V_NOTHING,
  };
  enum {
    Type_String = 1,
    Type_Integer,
    Type_Unknown,
  };

  InterpretResult run();
  void runtimeError(const std::string error);
  auto checkVariable(std::string name);
  void push(const uint32_t &type, const std::string &reg);
  void pop(const uint32_t &type, const std::string &reg);

  std::string assembly_filename;
  std::vector<stackVariable> stackVar;
  Bytecode *bytecode;
  uint32_t stackSize = -1; // The reason I say -1 is that the currentType[index]
                           // array should be the same as the stackSize

  std::vector<uint32_t> currentType;

  std::stringstream assembly_main;
  std::stringstream assembly_text;
  std::stringstream assembly_data;

  std::vector<uint32_t> opcode;

public:
  Generator(std::string filename);
  void free();
  InterpretResult main(Bytecode &pBytecode);
};
#endif