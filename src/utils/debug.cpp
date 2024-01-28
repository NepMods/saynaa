#include "debug.h"

Debug::Debug(Value value) : value(value.value), opcode(value.opcode), lines(value.lines) {
}
void Debug::disassemble(const std::string name) {
  std::cout << "== " << name << " ==" << std::endl;

  for (int offset = 0; offset < opcode.size();) {
    offset = disassembleInstruction(offset);
  }
}

int Debug::disassembleInstruction(int offset) {
  printf("%04d ", offset);

  if (offset > 0 &&
    lines[offset] == lines[offset - 1]) {
    std::cout << "   | ";
  } else {
    std::cout << "   " << lines[offset] << " ";
  }

  uint8_t instruction = opcode[offset];
  switch (instruction) {
    case OP_CONSTANT:
      return constantInstruction("OP_CONSTANT", offset);
    case OP_NIL:
      return simpleInstruction("OP_NIL", offset);
    case OP_TRUE:
      return simpleInstruction("OP_TRUE", offset);
    case OP_FALSE:
      return simpleInstruction("OP_FALSE", offset);
    case OP_POP:
      return simpleInstruction("OP_POP", offset);
    case OP_GET_LOCAL:
      return constantInstruction("OP_GET_LOCAL", offset);
    case OP_DEFINE_LOCAL:
      return constantInstruction("OP_DEFINE_LOCAL", offset);
    case OP_EQUAL:
      return simpleInstruction("OP_EQUAL", offset);
    case OP_GREATER:
      return simpleInstruction("OP_GREATER", offset);
    case OP_LESS:
      return simpleInstruction("OP_LESS", offset);
    case OP_ADD:
      return simpleInstruction("OP_ADD", offset);
    case OP_SUBTRACT:
      return simpleInstruction("OP_SUBTRACT", offset);
    case OP_MULTIPLY:
      return simpleInstruction("OP_MULTIPLY", offset);
    case OP_DIVIDE:
      return simpleInstruction("OP_DIVIDE", offset);
     case OP_NOT:
      return simpleInstruction("OP_NOT", offset);
    case OP_NEGATE:
      return simpleInstruction("OP_NEGATE", offset);
    case OP_PRINT:
      return simpleInstruction("OP_PRINT", offset);
    case OP_RETURN:
      return simpleInstruction("OP_RETURN", offset);
    default:
      std::cout << "Unknown opcode " << instruction << std::endl;
      return offset + 1;
  }
}

int Debug::simpleInstruction(const std::string name, int offset) {
  std::cout << name << std::endl;
  return offset + 1;
}

int Debug::constantInstruction(const std::string name, int offset) {
  uint8_t val = opcode[offset +1];
  std::cout << std::left << std::setw(16) << name << std::setw(4) << val << "'";

  // this code below doing: if value stored int (print int), else if stored string (print string)
  if(std::holds_alternative<int>(value[val])) {
    int output = std::get<int>(value[val]);
    std::cout << output;
  } else if (std::holds_alternative<std::string>(value[val])){
    std::string output = std::get<std::string>(value[val]);
    std::cout << output;
  }

  std::cout << "'" << std::endl;
  return offset + 2;
}