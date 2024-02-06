#ifndef lang_debug_h
#define lang_debug_h

#include "../shared/common.h"
#include "../shared/value.h"

class Debug {
  int disassembleInstruction(int offset);
  int simpleInstruction(const std::string name, int offset);
  int constantInstruction(const std::string name, bool isNumber, int offset);

public:
  Debug(Bytecode value);
  void disassemble(const std::string name);

  Bytecode bytecode;
};
#endif