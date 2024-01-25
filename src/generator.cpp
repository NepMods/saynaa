#include "common.h"
#include "compiler.h"
#include "generator.h"
#include <cstdarg>

Generator generator; 

Generator::Generator() {
    assembly_data << "section .data\n";
    assembly_text << "section .text\n";
    assembly_text << "global _start\n";
    
    assembly_text << "print_int:\n";
    assembly_text << "    mov    eax, edi\n";
    assembly_text << "    mov    ecx, 0xa\n";
    assembly_text << "    push   rcx\n";
    assembly_text << "    mov    rsi, rsp\n";
    assembly_text << "    sub    rsp, 16\n";
    assembly_text << "    .toascii_digit:\n";
    assembly_text << "        xor    edx, edx\n";
    assembly_text << "        div    ecx\n";
    assembly_text << "        add    edx, '0'\n";
    assembly_text << "        dec    rsi\n";
    assembly_text << "        mov    [rsi], dl\n";
    assembly_text << "        test   eax, eax\n";
    assembly_text << "        jnz    .toascii_digit\n";
    assembly_text << "    mov    eax, 1\n";
    assembly_text << "    mov    edi, 1\n";
    assembly_text << "    lea    edx, [rsp+16 + 1]\n";
    assembly_text << "    sub    edx, esi\n";
    assembly_text << "    syscall\n";
    assembly_text << "    add    rsp, 24\n";
    assembly_text << "    ret\n";

    assembly_main << "_start:\n";

}

void Generator::free() {
}

void Generator::runtimeError(const std::string error) {
  cout << error << endl;
}

void Generator::push(const std::string& reg) {
  assembly_main << "    push " << reg << "\n";
  stackSize++;
}

void Generator::pop(const std::string& reg) {
  assembly_main << "    pop " << reg << "\n";
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
            assembly_main << "    add rax, rbx\n";          \
        } else if(type == V_SUB) {                          \
            pop("rbx");                                     \
            pop("rax");                                     \
            assembly_main << "    sub rax, rbx\n";          \
        } else if(type == V_MUL) {                          \
            pop("rax");                                     \
            pop("rbx");                                     \
            assembly_main << "    mul rbx\n";               \
        } else if(type == V_DIV) {                          \
            pop("rbx");                                     \
            pop("rax");                                     \
            assembly_main << "    div rbx\n";               \
        }                                                   \
        push("rax");                                        \
    } while (false)
  
    for (;;) {
      uint8_t instruction;
      switch (instruction = READ_BYTE()) {
        case OP_CONSTANT: {
          assembly_main << "    mov rax, " << std::to_string(checkAndReturnValue(READ_CONSTANT())) << "\n";
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
          assembly_main << "    call print_int\n";
          break;
        }
        case OP_RETURN: {
          push("rax");

          assembly_main << "    mov rax, 60\n";
          assembly_main << "    xor rdi, rdi\n";
          assembly_main << "    syscall\n";

          std::ofstream outputFile("nasm.asm");
          outputFile << assembly_data.str();
          outputFile << assembly_text.str();
          outputFile << assembly_main.str();
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