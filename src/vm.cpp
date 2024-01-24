#include "common.h"
#include "compiler.h"
#include "vm.h"
#include <cstdarg>

VM vm; 

VM::VM() {
    assembly << "section .data\n";
    assembly << "section .text\n";
    assembly << "global _start\n";
    assembly << "_start:\n";
}

void VM::free() {
}

void VM::runtimeError(const std::string error) {
  cout << error << endl;
}

void VM::push(const std::string& reg) {
  assembly << "    push " << reg << "\n";
  stackSize++;
}

void VM::pop(const std::string& reg) {
  assembly << "    pop " << reg << "\n";
  stackSize--;
}

int VM::checkAndReturnValue(Value val) {
  if (!IS_NUMBER(val))
    runtimeError("Sorry must be numbers.");
  return AS_NUMBER(val);
}

InterpretResult VM::bytecode() {
  #define READ_BYTE() (*ip++)
  #define READ_CONSTANT() (chunk->constants.values[READ_BYTE()])
  #define BINARY_OP(valueType, op, type)                    \
    do {                                                    \
        if(type == V_ADD){                                  \
            pop("rax");                                     \
            pop("rbx");                                     \
            assembly << "    add rax, rbx\n";               \
        } else if(type == V_SUB) {                          \
            pop("rbx");                                     \
            pop("rax");                                     \
            assembly << "    sub rax, rbx\n";               \
        } else if(type == V_MUL) {                          \
            pop("rax");                                     \
            pop("rbx");                                     \
            assembly << "    mul rbx\n";                    \
        } else if(type == V_DIV) {                          \
            pop("rbx");                                     \
            pop("rax");                                     \
            assembly << "    div rbx\n";                    \
        }                                                   \
        push("rax");                                        \
    } while (false)
  
    for (;;) {
      uint8_t instruction;
      switch (instruction = READ_BYTE()) {
        case OP_CONSTANT: {
          assembly << "    mov rax, " << std::to_string(checkAndReturnValue(READ_CONSTANT())) << "\n";
          push("rax");
          
          break;
        }
        case OP_NIL:  break;
        case OP_TRUE:  break;
        case OP_FALSE:  break;
        case OP_EQUAL: {
          break;
        }
        case OP_GREATER:  BINARY_OP(BOOL_VAL, >, V_NOTHING); break;
        case OP_LESS:     BINARY_OP(BOOL_VAL, <, V_NOTHING); break;
        case OP_ADD:      BINARY_OP(NUMBER_VAL, +, V_ADD); break;
        case OP_SUBTRACT: BINARY_OP(NUMBER_VAL, -, V_SUB); break;
        case OP_MULTIPLY: BINARY_OP(NUMBER_VAL, *, V_MUL); break;
        case OP_DIVIDE:   BINARY_OP(NUMBER_VAL, /, V_DIV); break;
        case OP_NOT:
          break;
        case OP_NEGATE:
          break;
        case OP_RETURN: {
          push("rax");
          
          assembly << "    mov rax, 60\n";
          assembly << "    pop rdi\n";
          assembly << "    syscall\n";
          
          std::ofstream outputFile("nasm.asm");
          outputFile << assembly.str();
          outputFile.close();

          return INTERPRET_OK;
        }
      }
    }
  
  #undef READ_CONSTANT
  #undef BINARY_OP
  #undef READ_BYTE
}

InterpretResult VM::run(const std::string source) {
  Chunk ch;

  if (!compile(source, &ch)) {
    ch.free();
    return INTERPRET_COMPILE_ERROR;
  }

  vm.chunk = &ch;
  vm.ip = vm.chunk->code;

  InterpretResult result = bytecode();

  ch.free();
  return result;
}