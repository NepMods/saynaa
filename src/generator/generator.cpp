/*
 * Copyright (c) 2023-2024 Mohammed Abdifatah. All rights reserved.
 * Distributed Under The MIT License
 *
 * src/genrator/generator.cpp:
 *   Contains final x86_64 Nasm code generator from phase opcode
 */

#include "generator.h"
#include "macros.h"
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <ostream>
#include <sstream>

Generator::Generator(std::string filename) : assembly_filename(filename) {
  std::stringstream cur_lbl;
  current_label = &cur_lbl;
  assembly_data << "section .data\n";
  assembly_data << "    allVariable: dq 0\n";
  assembly_data << "    tmpValue: dq 0\n";
  assembly_data << "\n";
  assembly_data << "    TYPE_STR: equ 10\n";
  assembly_data << "    TYPE_INT: equ 20\n";

  *current_label << "section .text\n";
  *current_label << "global _start\n\n";
  push_label();

  *current_label << "\%macro allocateSpace 1\n";
  *current_label << "    push \%1\n";
  *current_label << "    call allocate\n";
  *current_label << "\%endmacro\n";
  *current_label << "\n";
  *current_label << "\%macro freeSpace 2\n";
  *current_label << "    push \%2\n";
  *current_label << "    push \%1\n";
  *current_label << "    call free\n";
  *current_label << "\%endmacro\n\n";
  push_label();

  *current_label << "allocate:; allocate(size)\n";
  *current_label << "    push rbp\n";
  *current_label << "    mov rbp, rsp\n";
  *current_label << "    mov rax, 9\n";
  *current_label << "    xor rdi, rdi\n";
  *current_label << "    mov rsi, qword[rbp+16]\n";
  *current_label << "    mov rdx, 3\n";
  *current_label << "    mov r10, 0x22\n";
  *current_label << "    xor r8, r8\n";
  *current_label << "    xor r9, r9\n";
  *current_label << "    syscall\n";
  *current_label << "    pop rbp\n";
  *current_label << "    \n";
  *current_label << "    ; remove stack parameter\n";
  *current_label << "    mov rbx, qword[rsp]\n";
  *current_label << "    add rsp, 16\n";
  *current_label << "    push rbx\n";
  *current_label << "    ret\n\n";
  push_label();

  *current_label << "free:; free(address, size)\n";
  *current_label << "    push rbp\n";
  *current_label << "    mov rbp, rsp\n";
  *current_label << "    mov rax, 11\n";
  *current_label << "    mov rdi, qword[rbp+16] ; address\n";
  *current_label << "    mov rsi, qword[rbp+24] ; size\n";
  *current_label << "    syscall\n";
  *current_label << "    pop rbp\n";
  *current_label << "    \n";
  *current_label << "    ; remove stack parameter\n";
  *current_label << "    mov rbx, qword[rsp]\n";
  *current_label << "    add rsp, 24\n";
  *current_label << "    push rbx\n";
  *current_label << "    ret\n\n";
  push_label();

  *current_label << "print:\n";
  *current_label << "    push rbp\n";
  *current_label << "    mov rbp, rsp\n";
  *current_label << "    mov bl, byte[rdi]\n";
  *current_label << "    mov rdi, qword[rdi+1]\n";
  *current_label << "    cmp bl, TYPE_STR\n";
  *current_label << "    jne .print_int\n";
  *current_label << "    .print_str:\n";
  *current_label << "        mov rax, rdi\n";
  *current_label << "        mov rbx, 0\n";
  *current_label << "        .print_str_count:\n";
  *current_label << "            inc rax\n";
  *current_label << "            inc rbx\n";
  *current_label << "            mov cl, [rax]\n";
  *current_label << "            cmp cl, 0\n";
  *current_label << "            jne .print_str_count\n";
  *current_label << "        mov rax, 1\n";
  *current_label << "        mov rsi, rdi\n";
  *current_label << "        mov rdi, 1\n";
  *current_label << "        mov rdx, rbx\n";
  *current_label << "        syscall\n";
  *current_label << "        jmp .end_print\n";
  *current_label << "    .print_int:\n";
  *current_label << "        mov r15, rdi\n";
  *current_label << "        allocateSpace 16\n";
  *current_label << "        mov rbx, rax\n";
  *current_label << "        mov ecx, 0xa\n";
  *current_label << "        add rax, 16\n";
  *current_label << "        mov rsi, rax\n";
  *current_label << "        mov byte[rsi], cl\n";
  *current_label << "        mov rax, r15\n";
  *current_label << "        .toascii_digit:\n";
  *current_label << "            xor edx, edx\n";
  *current_label << "            div ecx\n";
  *current_label << "            add edx, '0'\n";
  *current_label << "            dec rsi\n";
  *current_label << "            mov byte[rsi], dl\n";
  *current_label << "            test eax, eax\n";
  *current_label << "            jnz .toascii_digit\n";
  *current_label << "        mov rax, 1\n";
  *current_label << "        mov rdi, 1\n";
  *current_label << "        lea rdx, [rbx+16+1]\n";
  *current_label << "        sub rdx, rsi\n";
  *current_label << "        syscall\n";
  *current_label << "        freeSpace rbx, 16\n";
  *current_label << "    .end_print:\n";
  *current_label << "    pop rbp\n";
  *current_label << "    ret\n\n";
  push_label();

  assembly_main << "\n\nmain:\n";
  assembly_main << beg_label;
}

void Generator::free() {
}

auto Generator::checkVariable(std::string name, CodeContext* cCcontext) {
  if(cCcontext == nullptr) {

  return std::ranges::find_if(
      Ccontext->stackVar, [&](const stackVariable &var) { return var.name == name; });
  }

  return std::ranges::find_if(
      cCcontext->stackVar, [&](const stackVariable &var) { return var.name == name; });
}

void Generator::runtimeError(const std::string error) {
  std::cerr << error << std::endl;
  exit(1);
}

// copy *current_label value into array assembly_label,
// and clear *current_label
void Generator::push_label() {
  assembly_label.push_back(current_label->str());
  current_label->str(""); // clear
}

void Generator::BinaryOP(u_int32_t type) {
  *assembly_body << "    ; begin BinaryOP\n";

  get_tmpValue("rbx");
  get_tmpValue("rax");

  do {
    if (type == ADD_METHOD) {
      *assembly_body << "    add rax, rbx\n";
      *assembly_body << "    mov r15, rax\n";
    } else if (type == SUB_METHOD) {
      *assembly_body << "    sub rax, rbx\n";
      *assembly_body << "    mov r15, rax\n";
    } else if (type == MUL_METHOD) {
      *assembly_body << "    mul rbx\n";
      *assembly_body << "    mov r15, rax\n";
    } else if (type == DIV_METHOD) {
      *assembly_body << "    xor rdx, rdx\n";
      *assembly_body << "    div rbx\n";
      *assembly_body << "    mov r15, rax\n";
    }

    if (type == EQU_METHOD) {
      *assembly_body << "    cmp rax, rbx\n";
      *assembly_body << "    sete al\n";
      *assembly_body << "    movzx eax, al\n";
      *assembly_body << "    mov r15, rax\n";
    } else if (type == NEQ_METHOD) {
      *assembly_body << "    cmp rax, rbx\n";
      *assembly_body << "    setne al\n";
      *assembly_body << "    movzx eax, al\n";
      *assembly_body << "    mov r15, rax\n";
    }

    store_Variable("TYPE_INT", "r15");
    store_tmpValue();
    *assembly_body << "    ; End BinaryOP\n";
  } while (false);
}

void Generator::store_Variable(const std::string &type,
                               const std::string &val) {
  
  *assembly_body << "    allocateSpace 9\n";

  *assembly_body << "    mov byte[rax], " << type << "\n";
  *assembly_body << "    mov qword[rax+1], " << val << "\n";

}

void Generator::store_ptrAllVariable(int location) {
  *assembly_body << "    mov rbx, qword[allVariable]\n";
  *assembly_body << "    mov qword[rbx+" << location << "], rax\n";
  *assembly_body << "    mov rax, rbx\n";
}

void Generator::store_tmpValue() {
  *assembly_body << "    mov rbx, qword[tmpValue]\n";
  *assembly_body << "    mov qword[rbx+" << (total_tmpValue++) * 8
                 << "], rax\n";
  *assembly_body << "    mov rax, rbx\n";
}

void Generator::get_tmpValue(const std::string &reg, std::stringstream *ss,
                             int *temp_value) {
  if (ss == nullptr) {
    ss = assembly_body;
  }
  int local_total_tmpValue = 2;
  if (temp_value == nullptr) {
    local_total_tmpValue = total_tmpValue;
  }
  *ss << "    mov " << reg << ", qword[tmpValue]\n";
  *ss << "    mov " << reg << ", qword[" << reg << "+"
      << (--local_total_tmpValue) * 8 << "]\n";
  *ss << "    mov " << reg << ", qword[" << reg << "+1]\n";
}

uint32_t Generator::get_op() {
  if(index < opcode.size()) {
    return opcode[index];
  }
  return OP_NONE;
}
uint32_t Generator::next_op() {
  index++;
  return get_op();
}
uint32_t Generator::peek_op(int offset) {
  if(index+offset < opcode.size()) {
    return opcode[index+offset];
  }
  return OP_NONE;
}
void Generator::ignore_to(uint32_t opfcode) {
  while(peek_op() != opfcode) {
    next_op();
    if(get_op() == OP_NONE) {
      runtimeError("Unexpected end");
    }
  }
}
// generate x86_64 nasm assembly
InterpretResult Generator::run(uint32_t opcode, std::stringstream *stream, CodeContext *Ccontext) {
    assembly_body = current_label_stack.back();
    Ccontext = Pcontext.back();
    switch (opcode) {

      case OP_CONSTANT: {
        // OP_CONSTANT: is value holder
        auto bcode = bytecode->value[next_op()];
        if (peek_op() == OP_POP) {
          index++;
          return INTERPRET_OK;
        }

        *assembly_body << "    ; tmpValue\n";
        if (std::holds_alternative<int>(bcode)) {
          int value = std::get<int>(bcode);
          store_Variable("TYPE_INT", std::to_string(value));
        } else if (std::holds_alternative<std::string>(bcode)) {
          std::string value = std::get<std::string>(bcode);
          std::string rondomText = generateRandomText(7);

          // example: ?_rondom: db 0x48, ..., 0x00
          assembly_data << "    ?_" << rondomText << ": db "
                        << stringToHexDecimal(value);

          *assembly_body << "    lea r15, [?_" << rondomText << "]\n";
          store_Variable("TYPE_STR", "r15");
        }
        store_tmpValue();
        *assembly_body << "    ; tmpValue\n";
        break;
      }

      case OP_NOT:
        get_tmpValue("rax");
        *assembly_body << "    test rax, rax\n";
        *assembly_body << "    sete al\n";
        *assembly_body << "    movzx eax, al\n";
        *assembly_body << "    mov r15, rax\n";
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
      case OP_SET_LOCAL: {
        std::string name = bytecode->name[next_op()];
        auto iter = checkVariable(name, Ccontext);
        *assembly_body << "    ; setLocal\n";
        *assembly_body << "    mov rax, qword[rax+" << (--total_tmpValue) * 8
                      << "]\n";
        if (iter == Ccontext->stackVar.end()) {
          // not found: variable not defined before
          runtimeError("variable "+name+" not defined in "+Ccontext->name+"!");
        }

        store_ptrAllVariable((iter->variableLocation) * 8);
        *assembly_body << "    ; setLocal\n";
        break;
      }
      case OP_GET_LOCAL: {
        std::string name = bytecode->name[next_op()];
        if (peek_op() == OP_CALL) {
          index++; // OP_CALL
          index++; // How many parameters (now exists 0)
              // you can see in: opcode[i+1]
          // TODO: function has parameter
          *assembly_body << "    call " << name << "\n";
          return INTERPRET_OK;
        }
        auto iter = checkVariable(name, Ccontext);
        if (iter == Ccontext->stackVar.end()) {
          runtimeError("variable "+ name +" not defined "+Ccontext->name+"!");
        }
        *assembly_body << "    ; getLocal\n";
        *assembly_body << "    mov rax, qword[allVariable]\n";
        *assembly_body << "    mov rbx, qword[rax+" << iter->variableLocation * 8
                      << "]\n";
        *assembly_body << "    mov rax, rbx\n";
        store_tmpValue();
        *assembly_body << "    ; getLocal\n";
        break;
      }
      case OP_DEFINE_LOCAL: {
        std::string name = bytecode->name[next_op()];
        auto iter = checkVariable(name, Ccontext);
        *assembly_body << "    ; defineLocal\n";
        *assembly_body << "    mov rax, qword[rax+" << (--total_tmpValue) * 8
                      << "]\n";
        if (iter != Ccontext->stackVar.end()) {
          runtimeError("variable "+name+" is already defined in "+Ccontext->name+"!");

        } else {
          Ccontext->stackVar.push_back({name, total_allVariable});
          store_ptrAllVariable((total_allVariable++) * 8);
        }
        *assembly_body << "    ; defineLocal\n";
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
        *assembly_body << "    ; print\n";
        *assembly_body << "    mov rdi, qword[rax+" << (--total_tmpValue) * 8
                      << "]\n";
        *assembly_body << "    call print\n";
        *assembly_body << "    ; print\n";


        break;
      }
      case OP_BEG_FUNC: {
        stack += 1;
        std::stringstream *this_func = new std::stringstream();
        current_label_stack.push_back(this_func);

        std::string label_name = bytecode->name[next_op()];

        CodeContext *Current_context;
        if(Ccontext == GContext) {
          Current_context = new CodeContext();
          Current_context->name = label_name;
          Current_context->stackVar = {};
          Pcontext.push_back(Current_context);
        } else {
          Current_context = new CodeContext();
          Current_context->name = label_name;
          Current_context->stackVar.reserve(Ccontext->stackVar.size());
          std::copy(Ccontext->stackVar.begin(), Ccontext->stackVar.end(), std::back_inserter(Current_context->stackVar));
          Pcontext.push_back(Current_context);
        }

        *current_label_stack.back() << label_name << ":\n";
        *current_label_stack.back() << beg_label;

        run(get_op(), this_func, Current_context);
        while(next_op()!= OP_END_FUNC) {
          run(get_op(), current_label_stack.back(), Current_context);
        }

        stack -= 1;

        run(get_op(), current_label_stack.back(), Current_context);
        current_label_stack.pop_back();
        Pcontext.pop_back();
        assembly_label.push_back(this_func->str());


        break;
      }
      case OP_END_FUNC: {
        *assembly_body << end_label;

        break;
      }
      case OP_RETURN: {
        // solved duplicate end_lael
        break;
      }
      }
    return INTERPRET_OK;
}

InterpretResult Generator::main(Bytecode &pBytecode) {

  bytecode = &pBytecode;
  opcode = bytecode->opcode;

  GContext = new CodeContext();
  GContext->name = "main";
  Ccontext = GContext;
  Pcontext.push_back(Ccontext);

  assembly_body = &assembly_main;
  current_label_stack.push_back(&assembly_main);

  while(next_op() != OP_NONE) {
    run(get_op(), &assembly_main, GContext);
  }
  // InterpretResult result = run();

  std::stringstream assembly_start;
  assembly_start << "\n_start:\n";
  // allocate 80 bytes to store 10 variable's address
  assembly_start << "    allocateSpace 80\n";
  assembly_start << "    mov qword[allVariable], rax\n";
  assembly_start << "    allocateSpace 32\n";
  assembly_start << "    mov qword[tmpValue], rax\n";
  assembly_start << "    ; call main function\n";
  assembly_start << "    call main\n";
  assembly_start << "    ; exit\n";
  assembly_start << "    mov rax, qword[rax+" << (--total_tmpValue) * 8 << "]\n";
  assembly_start << "    mov rbx, qword[rax+1]\n";
  assembly_start << "    mov rdi, rbx\n";
  assembly_start << "    mov rax, 60\n";
  assembly_start << "    syscall\n";

  assembly_main << end_label;

  std::ofstream outputFile(assembly_filename);
  outputFile << assembly_data.str();
  for (int i = 0; i != assembly_label.size(); i++) {
    outputFile << assembly_label[i];
  }
  outputFile << assembly_main.str();
  outputFile << assembly_start.str();
  outputFile.close();

  return INTERPRET_OK;
}
