#include "common.h"
#include "compiler.h"
#include "generator.h"
#include <cstdarg>

Generator generator; 

Generator::Generator() {
    assembly << "section .data\n";
    assembly << "section .text\n";
    assembly << "global _start\n";
    
    assembly << "print_int:\n";
    assembly << "    mov    eax, edi\n";
    assembly << "    mov    ecx, 0xa\n";
    assembly << "    push   rcx\n";
    assembly << "    mov    rsi, rsp\n";
    assembly << "    sub    rsp, 16\n";
    assembly << "    .toascii_digit:\n";
    assembly << "        xor    edx, edx\n";
    assembly << "        div    ecx\n";
    assembly << "        add    edx, '0'\n";
    assembly << "        dec    rsi\n";
    assembly << "        mov    [rsi], dl\n";
    assembly << "        test   eax, eax\n";
    assembly << "        jnz    .toascii_digit\n";
    assembly << "    mov    eax, 1\n";
    assembly << "    mov    edi, 1\n";
    assembly << "    lea    edx, [rsp+16 + 1]\n";
    assembly << "    sub    edx, esi\n";
    assembly << "    syscall\n";
    assembly << "    add    rsp, 24\n";
    assembly << "    ret\n";

    assembly << "_start:\n";

}

void Generator::free() {
}

void Generator::runtimeError(const std::string error) {
  cout << error << endl;
}

void Generator::push(const std::string& reg) {
  assembly << "    push " << reg << "\n";
  stackSize++;
}

void Generator::pop(const std::string& reg) {
  assembly << "    pop " << reg << "\n";
  stackSize--;
}

int Generator::checkAndReturnValue(Value val) {
  if (!IS_NUMBER(val))
    runtimeError("Sorry must be numbers.");
  return AS_NUMBER(val);
}

InterpretResult Generator::bytecode() {
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
        case OP_PRINT: {
          pop("rdi");
          assembly << "    call print_int\n";
          break;
        }
        case OP_RETURN: {
          push("rax");
          
          assembly << "    mov rax, 60\n";
          assembly << "    xor rdi, rdi\n";
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

InterpretResult Generator::run(const std::string source) {
  Chunk ch;

  if (!compile(source, &ch)) {
    ch.free();
    return INTERPRET_COMPILE_ERROR;
  }

  generator.chunk = &ch;
  generator.ip = generator.chunk->code;

  InterpretResult result = bytecode();

  ch.free();
  return result;
}