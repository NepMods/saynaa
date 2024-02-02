#include "generator.h"

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
  variable.clear();
  value->free();
}

auto Generator::checkVariable(std::string name) {
  return std::ranges::find_if(variable, [&](const Variable& var) {
      return var.name == name;
  });
}

void Generator::runtimeError(const std::string error) {
  std::cerr << error << std::endl;
  exit(1);
}

void Generator::push(const std::string& reg) {
  assembly_main << "    push " << reg << "\n";
  stackSize++;
}

void Generator::pop(const std::string& reg) {
  assembly_main << "    pop " << reg << "\n";
  stackSize--;
}

InterpretResult Generator::run() {
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

    for(int i = 0; i != opcode.size(); i++) {
      switch (opcode[i]) {
        case OP_CONSTANT: {
          auto val = value->value[opcode[++i]];

          // this code below doing: if val stored int {...}, else if stored string {...}
          if(std::holds_alternative<int>(val)) {
            int output = std::get<int>(val);
            assembly_main << "    mov rax, " << std::to_string(output) << "\n";
            push("rax");
          } else if (std::holds_alternative<std::string>(val)){
            std::string output = std::get<std::string>(val);
            std::cout << "Soon adding: " << output << std::endl;
          }
          break;
        }
        case OP_NIL:  break;
        case OP_TRUE:  break;
        case OP_FALSE:  break;
        case OP_POP: /*pop();*/ break;
        case OP_GET_LOCAL: {
          auto val = value->value[opcode[++i]];

          // this code below doing: if val stored int {...}, else if stored string {...}
          if(std::holds_alternative<int>(val)) {
            int output = std::get<int>(val);
            runtimeError("expected an identifier!");
          } else if (std::holds_alternative<std::string>(val)){
            std::string output = std::get<std::string>(val);
            auto iter = checkVariable(output);
            if(iter == variable.end()) {
              runtimeError("variable not defined!"); // error is: var 10 = 29; 
            }
            std::stringstream offset;
            offset << "QWORD [rsp + " << (stackSize - iter->stackLocation) * 8 << "]";
            push(offset.str());
          }

          break;
        }
        case OP_DEFINE_LOCAL: {
          auto val = value->value[opcode[++i]];

          // this code below doing: if val stored int {...}, else if stored string {...}
          if(std::holds_alternative<int>(val)) {
            int output = std::get<int>(val);
            runtimeError("expected an identifier!");
          } else if (std::holds_alternative<std::string>(val)){
            std::string output = std::get<std::string>(val);
            if(variable.end() != checkVariable(output))
              runtimeError("variable before defined!");
            variable.push_back({output, stackSize});
          }
          break;
        }
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

  #undef BINARY_OP
  return INTERPRET_RUNTIME_ERROR;
}

InterpretResult Generator::main(const std::string source) {
  Value val;
  Parser parser;

  if (!parser.compile(source, &val)) {
    val.free();
    return INTERPRET_COMPILE_ERROR;
  }

  value = &val;
  opcode = value->opcode;

  InterpretResult result = run();

  val.free();
  return result;
}