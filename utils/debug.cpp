#include "debug.h"
bool debug_enabled = false;

Debug::Debug(Bytecode value) : bytecode(value) {}

void Debug::disassemble(const std::string name) {
  if (debug_enabled)
    std::cout << "== " << name << " ==" << std::endl;

  for (int offset = 0; offset < bytecode.opcode.size();) {
    offset = disassembleInstruction(offset);
  }

  bytecode.free();

  if (debug_enabled)
    std::cout << "== " << name << " ==" << std::endl;
}

int Debug::disassembleInstruction(int offset) {
  if (debug_enabled)
    printf("%04d ", offset);

  if (debug_enabled) {
    if (offset > 0 && bytecode.lines[offset] == bytecode.lines[offset - 1]) {
      std::cout << "   | ";
    } else {
      std::cout << "   " << bytecode.lines[offset] << " ";
    }
  }

  uint32_t instruction = bytecode.opcode[offset];
  switch (instruction) {
    case OP_CONSTANT:
      return constantInstructionValue("OP_CONSTANT", offset);
    case OP_JUMP_HERE:
      return constantInstructionValue("OP_JUMP_HERE", offset);
    case OP_NULL:
      return simpleInstruction("OP_NULL", offset);
    case OP_TRUE:
      return simpleInstruction("OP_TRUE", offset);
    case OP_FALSE:
      return simpleInstruction("OP_FALSE", offset);
    case OP_POP:
      return simpleInstruction("OP_POP", offset);
    case OP_SET_LOCAL:
      return constantInstructionName("OP_SET_LOCAL", offset);
    case OP_GET_LOCAL:
      return constantInstructionName("OP_GET_LOCAL", offset);
    case OP_DEFINE_LOCAL:
      return constantInstructionName("OP_DEFINE_LOCAL", offset);
    case OP_EQUAL:
      return simpleInstruction("OP_EQUAL", offset);
    case OP_NEQU:
      return simpleInstruction("OP_NEQU", offset);
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
    case OP_BEG_FUNC:
      return constantInstructionName("OP_BEG_FUNC", offset);
    case OP_END_FUNC:
      return simpleInstruction("OP_END_FUNC", offset);
    case OP_CALL:
      return callInstruction("OP_CALL", offset);
    case OP_JUMP_IF_NOT:
      return jumpInstruction("OP_JUMP_IF_NOT", offset);
    case OP_JUMP:
      return jumpInstruction("OP_JUMP", offset);
    case OP_RETURN:
      return simpleInstruction("OP_RETURN", offset);
    case OP_TEST:
      return simpleInstruction("OP_TEST", offset);
    case OP_NONE:
      return simpleInstruction("OP_NONE", offset);
    default:
      if (debug_enabled)
        std::cout << "Unknown opcode " << instruction << std::endl;
      return offset + 1;
  }
}

int Debug::simpleInstruction(const std::string name, int offset) {
  if (debug_enabled)
    std::cout << name << std::endl;
  return offset + 1;
}

int Debug::callInstruction(const std::string name, int offset) {
  uint32_t val = bytecode.opcode[offset + 1];
  if (debug_enabled)
    std::cout << std::left << std::setw(18) << name << std::setw(4) << val
              << std::endl;
  return offset + 2;
}

int Debug::jumpInstruction(const std::string name, int offset) {
  uint32_t val = bytecode.opcode[offset + 1];
  if (debug_enabled)
    std::cout << std::left << std::setw(18) << name << std::setw(4) << ""
              << "'goto:" << val << "'" << std::endl;
  return offset + 2;
}

int Debug::constantInstructionValue(const std::string name, int offset) {
  uint32_t val = bytecode.opcode[offset + 1];
  if (debug_enabled) {
    std::cout << std::left << std::setw(18) << name << std::setw(4) << val << "'";
    if (std::holds_alternative<int>(bytecode.value[val])) {
      std::cout << std::get<int>(bytecode.value[val]);
    } else if (std::holds_alternative<std::string>(bytecode.value[val])) {
      std::cout << std::get<std::string>(bytecode.value[val]);
    }
    std::cout << "'" << std::endl;
  }
  return offset + 2;
}

int Debug::constantInstructionName(const std::string name, int offset) {
  uint32_t val = bytecode.opcode[offset + 1];
  if (debug_enabled) {
    std::cout << std::left << std::setw(18) << name << std::setw(4) << val << "'";
    std::cout << bytecode.name[val];
    std::cout << "'" << std::endl;
  }
  return offset + 2;
}
