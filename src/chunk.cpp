#include "chunk.h"
#include "memory.h"
#include <c++/10/iomanip>

Chunk::Chunk() : count(0), lines(NULL), capacity(0), code(NULL) {

}

void Chunk::write(uint8_t byte, int line) {
    if (capacity < count + 1) {
        int oldCapacity = capacity;
        capacity = GROW_CAPACITY(oldCapacity);
        code = GROW_ARRAY(uint8_t, code, oldCapacity, capacity);
        lines = GROW_ARRAY(int, lines, oldCapacity, capacity);
    }

    code[count] = byte;
    lines[count] = line;
    count++;
}

 int Chunk::addConstant(Value value){
    constants.write(value);
    return constants.count - 1;
 }

void Chunk::free(){
    FREE_ARRAY(uint8_t, code, capacity);
    FREE_ARRAY(int, lines, capacity);
    constants.free();

    code = NULL;
    capacity = 0;
    lines = NULL;
    count = 0;
}

void Chunk::disassemble(const std::string name) {
  cout << "== " << name << " ==" << endl;

  for (int offset = 0; offset < count;) {
    offset = disassembleInstruction(offset);
  }
}

int Chunk::disassembleInstruction(int offset) {
  printf("%04d ", offset);
  //cout << setw(4) << setfill('0') << offset << " ";

  if (offset > 0 &&
      lines[offset] == lines[offset - 1]) {
    cout << "   | ";
  } else {
    cout << setw(4) << lines[offset] << " ";
  }

  uint8_t instruction = code[offset];
  switch (instruction) {
    case OP_CONSTANT:
      return constantInstruction("OP_CONSTANT", offset);
    case OP_NIL:
      return simpleInstruction("OP_NIL", offset);
    case OP_TRUE:
      return simpleInstruction("OP_TRUE", offset);
    case OP_FALSE:
      return simpleInstruction("OP_FALSE", offset);
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
    case OP_RETURN:
      return simpleInstruction("OP_RETURN", offset);
    default:
      cout << "Unknown opcode " << instruction << endl;
      return offset + 1;
  }
}

int Chunk::simpleInstruction(const std::string name, int offset) {
  cout << name << endl;
  return offset + 1;
}

int Chunk::constantInstruction(const std::string name, int offset) {
  uint8_t constant = code[offset + 1];
  cout << left << setw(16) << name << setw(4) << constant << "'";
  constants.printValue(constants.values[constant]);
  cout << "'" << endl;
  return offset + 2;
}