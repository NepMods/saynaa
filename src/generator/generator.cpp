#include "generator.h"

Generator::Generator() {
  assembly_data << "section .data\n";
  assembly_text << "section .text\n";
  assembly_text << "global _start\n";

  assembly_text << "\nprint_int:\n";
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
  assembly_text << "\nprint_str:\n";
  assembly_text << "    push rbp\n";
  assembly_text << "    mov rbp, rsp\n";
  assembly_text << "    mov rax, qword[rbp+16]\n";
  assembly_text << "    mov rbx, 0\n";
  assembly_text << "    .print_loop:\n";
  assembly_text << "        inc rax\n";
  assembly_text << "        inc rbx\n";
  assembly_text << "        mov cl, [rax]\n";
  assembly_text << "        cmp cl, 0\n";
  assembly_text << "        jne .print_loop\n";
  assembly_text << "    mov rax, 1\n";
  assembly_text << "    mov rdi, 1\n";
  assembly_text << "    mov rsi, qword[rbp+16]\n";
  assembly_text << "    mov rdx, rbx\n";
  assembly_text << "    syscall\n";
  assembly_text << "    pop rbp\n";
  assembly_text << "    ret\n";

  assembly_main << "\n_start:\n";
}

void Generator::free() {
  stackVar.clear();
  bytecode->free();
}

auto Generator::checkVariable(std::string name) {
  return std::ranges::find_if(
      stackVar, [&](const stackVariable &var) { return var.name == name; });
}

void Generator::runtimeError(const std::string error) {
  std::cerr << error << std::endl;
  exit(1);
}

void Generator::push(const uint32_t &type, const std::string &reg) {
  assembly_main << "    push " << reg << "\n";
  currentType.push_back(type);
  stackSize++;
}

void Generator::pop(const uint32_t &type, const std::string &reg) {
  assembly_main << "    pop " << reg << "\n";
  currentType.pop_back();
  stackSize--;
}

InterpretResult Generator::run() {
#define BINARY_OP(valueType, op, type)                                         \
  do {                                                                         \
    if (type == V_ADD) {                                                       \
      pop(Type_Integer, "rax");                                                \
      pop(Type_Integer, "rbx");                                                \
      assembly_main << "    add rax, rbx\n";                                   \
    } else if (type == V_SUB) {                                                \
      pop(Type_Integer, "rbx");                                                \
      pop(Type_Integer, "rax");                                                \
      assembly_main << "    sub rax, rbx\n";                                   \
    } else if (type == V_MUL) {                                                \
      pop(Type_Integer, "rax");                                                \
      pop(Type_Integer, "rbx");                                                \
      assembly_main << "    mul rbx\n";                                        \
    } else if (type == V_DIV) {                                                \
      pop(Type_Integer, "rbx");                                                \
      pop(Type_Integer, "rax");                                                \
      assembly_main << "    div rbx\n";                                        \
    }                                                                          \
    push(Type_Integer, "rax");                                                 \
  } while (false)

  for (int i = 0; i != opcode.size(); i++) {
    switch (opcode[i]) {
    case OP_CONSTANT: {
      auto bcode = bytecode->value[opcode[++i]];

      // this code below doing: if bcode stored int {...}, else if stored string
      // {...}
      if (std::holds_alternative<int>(bcode)) {
        int output = std::get<int>(bcode);
        assembly_main << "    mov rax, " << std::to_string(output) << "\n";
        push(Type_Integer, "rax");
      } else if (std::holds_alternative<std::string>(bcode)) {
        std::string output = std::get<std::string>(bcode);
        std::string rondomText = generateRandomText(7);
        assembly_data << "    " << rondomText << " dw " << output << ",0\n";
        push(Type_String, rondomText);
      }
      break;
    }
    case OP_NULL:
      break;
    case OP_TRUE:
      break;
    case OP_FALSE:
      break;
    case OP_POP:
      pop(Type_Unknown, "rax");
      break;
    case OP_GET_LOCAL: {
      std::string name = bytecode->name[opcode[++i]];
      auto iter = checkVariable(name);
      if (iter == stackVar.end()) {
        runtimeError("variable not defined!"); // error is: var 10 = 29;
      }
      std::stringstream offset;
      uint32_t location = stackSize - iter->stackLocation;
      offset << "qword [rsp + " << location * 8 << "]";
      push(currentType[stackSize - location], offset.str());
      break;
    }
    case OP_DEFINE_LOCAL: {
      std::string name = bytecode->name[opcode[++i]];
      if (stackVar.end() != checkVariable(name))
        runtimeError("variable before defined!");
      stackVar.push_back({name, stackSize});
      break;
    }
    case OP_EQUAL: {
      break;
    }
    case OP_GREATER:
      BINARY_OP(BOOL_VAL, >, V_NOTHING);
      break;
    case OP_LESS:
      BINARY_OP(BOOL_VAL, <, V_NOTHING);
      break;
    case OP_ADD:
      BINARY_OP(NUMBER_VAL, +, V_ADD);
      break;
    case OP_SUBTRACT:
      BINARY_OP(NUMBER_VAL, -, V_SUB);
      break;
    case OP_MULTIPLY:
      BINARY_OP(NUMBER_VAL, *, V_MUL);
      break;
    case OP_DIVIDE:
      BINARY_OP(NUMBER_VAL, /, V_DIV);
      break;
    case OP_NOT:
      break;
    case OP_NEGATE:
      break;
    case OP_PRINT: {
      if (currentType[stackSize] == Type_Integer) {
        pop(Type_Integer, "rdi");
        assembly_main << "    call print_int\n";
      } else if (currentType[stackSize] == Type_String) {
        assembly_main << "    call print_str\n";
      }
      break;
    }
    case OP_RETURN: {

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

InterpretResult Generator::main(Bytecode &pBytecode) {
  bytecode = &pBytecode;
  opcode = bytecode->opcode;

  InterpretResult result = run();
  return result;
}