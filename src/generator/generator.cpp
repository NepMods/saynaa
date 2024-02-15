/*
 * Copyright (c) 2023-2024 Mohammed Abdifatah. All rights reserved.
 * Distributed Under The MIT License
 *
 * src/genrator/generator.cpp:
 *   Contains final x86_64 Nasm code generator from phase opcode
 */

#include "generator.h"

Generator::Generator(std::string filename) : assembly_filename(filename) {

  assembly_data << "section .data\n";
  assembly_data << "    allVariable: dq 0\n";
  assembly_data << "    tmpValue: dq 0\n";
  assembly_data << "\n";
  assembly_data << "    TYPE_STR: equ 10\n";
  assembly_data << "    TYPE_INT: equ 20\n";

  assembly_text << "section .text\n";
  assembly_text << "global _start\n\n";

  assembly_text << "\%macro allocateSpace 1\n";
  assembly_text << "    push \%1\n";
  assembly_text << "    call allocate\n";
  assembly_text << "\%endmacro\n";
  assembly_text << "\n";
  assembly_text << "\%macro freeSpace 2\n";
  assembly_text << "    push \%2\n";
  assembly_text << "    push \%1\n";
  assembly_text << "    call free\n";
  assembly_text << "\%endmacro\n\n";

  assembly_text << "allocate:; allocate(size)\n";
  assembly_text << "    push rbp\n";
  assembly_text << "    mov rbp, rsp\n";
  assembly_text << "    mov rax, 9\n";
  assembly_text << "    xor rdi, rdi\n";
  assembly_text << "    mov rsi, qword[rbp+16]\n";
  assembly_text << "    mov rdx, 3\n";
  assembly_text << "    mov r10, 0x22\n";
  assembly_text << "    xor r8, r8\n";
  assembly_text << "    xor r9, r9\n";
  assembly_text << "    syscall\n";
  assembly_text << "    pop rbp\n";
  assembly_text << "    \n";
  assembly_text << "    ; remove stack parameter\n";
  assembly_text << "    mov rbx, qword[rsp]\n";
  assembly_text << "    add rsp, 16\n";
  assembly_text << "    push rbx\n";
  assembly_text << "    ret\n\n";

  assembly_text << "free:; free(address, size)\n";
  assembly_text << "    push rbp\n";
  assembly_text << "    mov rbp, rsp\n";
  assembly_text << "    mov rax, 11\n";
  assembly_text << "    mov rdi, qword[rbp+16] ; address\n";
  assembly_text << "    mov rsi, qword[rbp+24] ; size\n";
  assembly_text << "    syscall\n";
  assembly_text << "    pop rbp\n";
  assembly_text << "    \n";
  assembly_text << "    ; remove stack parameter\n";
  assembly_text << "    mov rbx, qword[rsp]\n";
  assembly_text << "    add rsp, 24\n";
  assembly_text << "    push rbx\n";
  assembly_text << "    ret\n\n";

  assembly_main << "print:\n";
  assembly_main << "    push rbp\n";
  assembly_main << "    mov rbp, rsp\n";
  assembly_main << "    mov bl, byte[rdi]\n";
  assembly_main << "    mov rdi, qword[rdi+1]\n";
  assembly_main << "    cmp bl, TYPE_STR\n";
  assembly_main << "    jne .print_int\n";
  assembly_main << "    .print_str:\n";
  assembly_main << "        mov rax, rdi\n";
  assembly_main << "        mov rbx, 0\n";
  assembly_main << "        .print_str_count:\n";
  assembly_main << "            inc rax\n";
  assembly_main << "            inc rbx\n";
  assembly_main << "            mov cl, [rax]\n";
  assembly_main << "            cmp cl, 0\n";
  assembly_main << "            jne .print_str_count\n";
  assembly_main << "        mov rax, 1\n";
  assembly_main << "        mov rsi, rdi\n";
  assembly_main << "        mov rdi, 1\n";
  assembly_main << "        mov rdx, rbx\n";
  assembly_main << "        syscall\n";
  assembly_main << "        jmp .end_print\n";
  assembly_main << "    .print_int:\n";
  assembly_main << "        mov r15, rdi\n";
  assembly_main << "        allocateSpace 16\n";
  assembly_main << "        mov rbx, rax\n";
  assembly_main << "        mov ecx, 0xa\n";
  assembly_main << "        add rax, 16\n";
  assembly_main << "        mov rsi, rax\n";
  assembly_main << "        mov byte[rsi], cl\n";
  assembly_main << "        mov rax, r15\n";
  assembly_main << "        .toascii_digit:\n";
  assembly_main << "            xor edx, edx\n";
  assembly_main << "            div ecx\n";
  assembly_main << "            add edx, '0'\n";
  assembly_main << "            dec rsi\n";
  assembly_main << "            mov byte[rsi], dl\n";
  assembly_main << "            test eax, eax\n";
  assembly_main << "            jnz .toascii_digit\n";
  assembly_main << "        mov rax, 1\n";
  assembly_main << "        mov rdi, 1\n";
  assembly_main << "        lea rdx, [rbx+16+1]\n";
  assembly_main << "        sub rdx, rsi\n";
  assembly_main << "        syscall\n";
  assembly_main << "        freeSpace rbx, 16\n";
  assembly_main << "    .end_print:\n";
  assembly_main << "    pop rbp\n";
  assembly_main << "    ret\n\n";

  assembly_main << "main:\n";
  assembly_main << "    push rbp\n";
  assembly_main << "    mov rbp, rsp\n";
  assembly_main << "    ; natural code\n";

  assembly_mainend << "    pop rbp\n";
  assembly_mainend << "    ret\n";
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

void Generator::BinaryOP(u_int32_t type) {
  assembly_main << "    ; begin BinaryOP\n";

  get_tmpValue("rbx");
  get_tmpValue("rax");

  do {
    if (type == ADD_METHOD) {
      assembly_main << "    add rax, rbx\n";
      assembly_main << "    mov r15, rax\n";
    } else if (type == SUB_METHOD) {
      assembly_main << "    sub rax, rbx\n";
      assembly_main << "    mov r15, rax\n";
    } else if (type == MUL_METHOD) {
      assembly_main << "    mul rbx\n";
      assembly_main << "    mov r15, rax\n";
    } else if (type == DIV_METHOD) {
      assembly_main << "    mov rcx, rax\n";
      assembly_main << "    mov rax, rbx\n";
      assembly_main << "    div rcx\n";
      assembly_main << "    mov r15, rax\n";
    }

    if (type == EQU_METHOD) {
      assembly_main << "    cmp rax, rbx\n";
      assembly_main << "    sete al\n";
      assembly_main << "    movzx eax, al\n";
      assembly_main << "    mov r15, rax\n";
    } else if (type == NEQ_METHOD) {
      assembly_main << "    cmp rax, rbx\n";
      assembly_main << "    setne al\n";
      assembly_main << "    movzx eax, al\n";
      assembly_main << "    mov r15, rax\n";
    }

    store_Variable("TYPE_INT", "r15");
    store_tmpValue();
    assembly_main << "    ; End BinaryOP\n";
  } while (false);
}

void Generator::store_Variable(const std::string &type,
                               const std::string &val) {
  assembly_main << "    allocateSpace 9\n";
  assembly_main << "    mov byte[rax], " << type << "\n";
  assembly_main << "    mov qword[rax+1], " << val << "\n";
}

void Generator::store_ptrAllVariable(int location) {
  assembly_main << "    mov rbx, qword[allVariable]\n";
  assembly_main << "    mov qword[rbx+" << location << "], rax\n";
  assembly_main << "    mov rax, rbx\n";
}

void Generator::store_tmpValue() {
  assembly_main << "    mov rbx, qword[tmpValue]\n";
  assembly_main << "    mov qword[rbx+" << (total_tmpValue++) * 8 << "], rax\n";
  assembly_main << "    mov rax, rbx\n";
}

void Generator::get_tmpValue(const std::string &reg) {
  assembly_main << "    mov " << reg << ", qword[tmpValue]\n";
  assembly_main << "    mov " << reg << ", qword[" << reg << "+"
                << (--total_tmpValue) * 8 << "]\n";
  assembly_main << "    mov " << reg << ", qword[" << reg << "+1]\n";
}

InterpretResult Generator::run() {

  for (int i = 0; i != opcode.size(); i++) {
    switch (opcode[i]) {
    case OP_CONSTANT: {
      auto bcode = bytecode->value[opcode[++i]];
      if (opcode[i + 1] == OP_POP) {
        i++;
        continue;
      }

      assembly_main << "    ; tmpValue\n";

      if (std::holds_alternative<int>(bcode)) {
        int value = std::get<int>(bcode);
        store_Variable("TYPE_INT", std::to_string(value));
      } else if (std::holds_alternative<std::string>(bcode)) {
        std::string value = std::get<std::string>(bcode);
        std::string rondomText = generateRandomText(7);
        assembly_data << "    " << rondomText << ": dw " << value << ",0\n";

        assembly_main << "    lea r15, [" << rondomText << "]\n";
        store_Variable("TYPE_STR", "r15");
      }
      store_tmpValue();
      assembly_main << "    ; tmpValue\n";
      break;
    }
    case OP_NOT:
      get_tmpValue("rax");
      assembly_main << "    test rax, rax\n";
      assembly_main << "    sete al\n";
      assembly_main << "    movzx eax, al\n";
      assembly_main << "    mov r15, rax\n";
      store_Variable("TYPE_INT", "r15");
      store_tmpValue();
      break;
    case OP_NULL:
      store_Variable("TYPE_INT", std::to_string(0));
      store_tmpValue();
      break;
    case OP_TRUE:
      store_Variable("TYPE_INT", std::to_string(1));
      store_tmpValue();
      break;
    case OP_FALSE:
      store_Variable("TYPE_INT", std::to_string(0));
      store_tmpValue();
      break;
    case OP_POP:
      --total_tmpValue;
      break;
    case OP_GET_LOCAL: {
      std::string name = bytecode->name[opcode[++i]];
      auto iter = checkVariable(name);
      if (iter == stackVar.end()) {
        runtimeError("variable not defined!");
      }
      assembly_main << "    ; getLocal\n";
      assembly_main << "    mov rax, qword[allVariable]\n";
      assembly_main << "    mov rbx, qword[rax+" << iter->variableLocation * 8
                    << "]\n";
      assembly_main << "    mov rax, rbx\n";
      store_tmpValue();
      assembly_main << "    ; getLocal\n";
      break;
    }
    case OP_DEFINE_LOCAL: {
      std::string name = bytecode->name[opcode[++i]];
      auto iter = checkVariable(name);
      assembly_main << "    ; defineLocal\n";
      assembly_main << "    mov rax, qword[rax+" << (--total_tmpValue) * 8
                    << "]\n";
      if (iter != stackVar.end()) {
        store_ptrAllVariable((iter->variableLocation) * 8);
      } else {
        stackVar.push_back({name, total_allVariable});
        store_ptrAllVariable((total_allVariable++) * 8);
      }
      assembly_main << "    ; defineLocal\n";
      break;
    }
    case OP_ADD:
      BinaryOP(ADD_METHOD);
      break;
    case OP_SUBTRACT:
      BinaryOP(SUB_METHOD);
      break;
    case OP_MULTIPLY:
      BinaryOP(MUL_METHOD);
      break;
    case OP_DIVIDE:
      BinaryOP(DIV_METHOD);
      break;
    case OP_EQUAL:
      BinaryOP(EQU_METHOD);
      break;
    case OP_NEQU:
      BinaryOP(NEQ_METHOD);
      break;
    case OP_PRINT: {
      assembly_main << "    ; print\n";
      assembly_main << "    mov rdi, qword[rax+" << (--total_tmpValue) * 8
                    << "]\n";
      assembly_main << "    call print\n";
      assembly_main << "    ; print\n";
      break;
    }
    }
  }
  return INTERPRET_OK;
}

InterpretResult Generator::main(Bytecode &pBytecode) {
  bytecode = &pBytecode;
  opcode = bytecode->opcode;

  InterpretResult result = run();

  std::stringstream assembly_start;
  assembly_start << "\n_start:\n";
  assembly_start << "    allocateSpace 80\n";
  assembly_start << "    mov qword[allVariable], rax\n";
  assembly_start << "    allocateSpace 32\n";
  assembly_start << "    mov qword[tmpValue], rax\n";
  assembly_start << "    ; call main function\n";
  assembly_start << "    call main\n";
  assembly_start << "    ; exit\n";
  assembly_start << "    mov rax, 60\n";
  assembly_start << "    xor rdi, rdi\n";
  assembly_start << "    syscall\n";

  std::ofstream outputFile(assembly_filename);
  outputFile << assembly_data.str();
  outputFile << assembly_text.str();
  outputFile << assembly_main.str();
  outputFile << assembly_mainend.str();
  outputFile << assembly_start.str();
  outputFile.close();

  return result;
}
