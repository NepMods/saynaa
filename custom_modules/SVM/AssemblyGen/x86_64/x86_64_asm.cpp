//
// Created by arjun on 7/21/25.
//

#include "x86_64_asm.h"

#include <fstream>
#include <iostream>

x86_64_register_manager main_reigster_manager = x86_64_register_manager();
static const char* param_regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
std::string x86_64_register_manager::hold_tmp() {
    held = (held + 1) % temp_registers.size();
    return temp_registers[held];
}

std::string x86_64_register_manager::release_tmp(int i) {
    held = (held - i) % temp_registers.size();
    std::string reg = temp_registers[held];
    // printf("Released %d\n", held);
    held--;
    return reg;
}

std::string x86_64_register_manager::hold_and_release_tmp() {
    std::string reg = hold_tmp();
    return release_tmp(); // you may want to use `reg` here instead
}
std::string x86_64_register_manager::pull_tmp()
{
    return temp_registers[held];
}

void x86_64_symbol::add_text(std::string text, int indent)
{

    add_asm_line(text, &this->text_bottom, indent);
}

void x86_64_symbol::add_text_top(std::string text, int indent)
{

    add_asm_line(text, &this->text_top, indent);
}

void x86_64_symbol::initialize(std::string name)
{

    this->name = name;
    exit_label = name + "_exit";
    add_text_top(name +":");
    add_text_top("push rbp", 1);
    add_text_top("mov rbp, rsp", 1);
}

void x86_64_symbol::finalize()
{
    if (finalized)
    {
        return;
    }
    if (!returned)
    {
        add_text("mov rax, 0", 1);
    }
    add_text("."+exit_label+":", 1);

    if (this->current_base_pointer < 0)
    {
        add_text_top("sub rsp, "+std::to_string((0-this->current_base_pointer)*8), 1);
        add_text("add rsp, "+std::to_string((0-this->current_base_pointer)*8), 1);
    }
    add_text("pop rbp", 1);
    add_text("ret", 1);
    finalized = true;
}
void x86_64_symbol::asm_call(std::string name, int param_size, std::vector<x86_64_symbol> list_symbols)
{
    bool found = false;
    for (auto symbol : list_symbols)
    {
        if (symbol.name == name && symbol.parameter_size == param_size)
        {
            found = true;
            add_text("call " + name, 1);
            add_text("mov "+main_reigster_manager.hold_tmp()+", rax", 1);
            if (param_size > 6)
            {
                add_text("add rsp, "+ std::to_string((param_size - 6) * 8), 1);
            }
        }
    }
    if (!found)
    {
        runtimeError("Cant find function named " + name + " with parameter size "+ std::to_string(param_size));
    }
}
void x86_64_symbol::asm_raw_line(std::string data)
{
    add_text(data, 1);
}
void x86_64_symbol::asm_syscall(int syscall)
{
    add_text("mov rax, "+std::to_string(syscall), 1);
    add_text("syscall", 1);
}
void x86_64_symbol::return_l()
{
    auto reg = main_reigster_manager.release_tmp();
    add_text("mov rax, " + reg, 1);
    add_text("jmp ."+exit_label, 1);
    returned = true;
}

void x86_64_symbol::add_temp_var(std::string value)
{
    add_text("mov "+main_reigster_manager.hold_tmp()+", "+value, 1);
}

void x86_64_symbol::push_variable(std::string name, bool isMain)
{
    x86_64_variable variable;
    variable.name = name;
    variable.type = NUMBER;
    variable.base_pointer = --current_base_pointer;

    if ((!isMain && name == "main") || (name != "main"))
    {
        add_text("mov qword [rbp+"+std::to_string(variable.base_pointer * 8)+"], "+main_reigster_manager.hold_and_release_tmp()+"", 1);
    }
    variables.push_back(variable);

}
void x86_64_symbol::runtimeError(std::string text)
{
    //Exit with message
        std::cerr << text << std::endl;
        exit(1);
}
void x86_64_symbol::add_parameter(std::string name, std::string def_value)
{
    x86_64_variable thisParam;
    thisParam.name = name;
    thisParam.base_pointer = (parameter_size);
    thisParam.type = NUMBER;
    parameters.push_back(thisParam);
    parameter_size++;
}

void x86_64_symbol::push_call_parameter(std::string value, int size)
{
    if (size < 6)
    {
        add_text("mov " + std::string(param_regs[size]) + ", " + value, 1);
    }
    else
    {
        add_text("mov "+main_reigster_manager.hold_and_release_tmp()+", " + value, 1);
        add_text("push "+main_reigster_manager.hold_and_release_tmp()+"", 1);
    }
}
void x86_64_symbol::set_variable(std::string name, std::string value, std::vector<x86_64_variable> &g_variables, bool isMain)
{
    bool found = false;
    x86_64_variable variable;

    for (auto var : variables)
    {
        if (var.name == name)
        {
            found = true;
            variable = var;
        }
    }

    if (!found)
    {
        for (auto var : (!isMain && this->name == "main") ? variables : g_variables )
        {
            printf("%s", var.name.c_str());
            if (var.name == name)
            {

                add_text("mov [rel "+var.name+"], "+main_reigster_manager.hold_and_release_tmp()+"", 1);
                return;
            }
        }
    }
    if (!found)
    {
        for (auto var : parameters)
        {
            if (var.name == name)
            {
                printf("bp: %d", var.base_pointer);
                if (var.base_pointer < 6)
                {
                    add_text("mov "+std::string(param_regs[var.base_pointer])+", "+main_reigster_manager.hold_and_release_tmp()+"", 1);
                } else
                {
                    int position = (parameter_size) - var.base_pointer;
                    add_text("mov [rbp + "+std::to_string((position + 1) * 8)+"], "+main_reigster_manager.hold_and_release_tmp()+"", 1);
                }

                return;
            }
        }
    }
    if (!found)
    {
        runtimeError("Variable "+name+" cant be found at symbol "+this->name);
    }

    add_text("mov  [rbp+"+std::to_string(variable.base_pointer * 8)+"], "+main_reigster_manager.hold_and_release_tmp()+"", 1);

}

void x86_64_symbol::get_variable(std::string name, std::vector<x86_64_variable> &g_variables, bool isMain)
{
    bool found = false;
    x86_64_variable variable;

        for (auto var : variables)
        {
            if (var.name == name)
            {
                found = true;
                variable = var;
            }
        }

    if (!found)
    {
        for (auto var : (!isMain && this->name == "main") ? variables : g_variables )
        {
            if (var.name == name)
            {

                add_text("mov "+main_reigster_manager.hold_tmp()+", [rel "+var.name+"]", 1);
                return;
            }
        }
    }
    if (!found)
    {
        for (auto var : parameters)
        {
            if (var.name == name)
            {
                printf("bp: %d", var.base_pointer);
                if (var.base_pointer < 6)
                {
                    add_text("mov "+main_reigster_manager.hold_tmp()+", "+std::string(param_regs[var.base_pointer])+"", 1);
                } else
                {
                    int position = (parameter_size) - var.base_pointer;
                    add_text("mov "+main_reigster_manager.hold_tmp()+", [rbp + "+std::to_string((position + 1) * 8)+"]", 1);
                }

                return;
            }
        }
    }
    if (!found)
    {
        runtimeError("Variable "+name+" cant be found at symbol "+this->name);
    }

    add_text("mov "+main_reigster_manager.hold_tmp()+", [rbp+"+std::to_string(variable.base_pointer * 8)+"]", 1);

}

void x86_64_symbol::binary_op(BINARY_OP op)
{
    auto reg1 = main_reigster_manager.pull_tmp();
    auto reg2 = main_reigster_manager.release_tmp();

    if (op == BINARY_OP_ADD)
    {
        add_text("add "+reg1+", "+reg2, 1);
        add_text("mov "+main_reigster_manager.hold_tmp()+", "+reg1, 1);
    }
    if (op == BINARY_OP_SUBTRACT)
    {
        add_text("sub "+reg2+", "+reg1, 1);
        add_text("mov "+main_reigster_manager.hold_tmp()+", "+reg2, 1);
    }
    if (op == BINARY_OP_MULTIPLY)
    {
        add_text("imul "+reg1+", "+reg2, 1);
        add_text("mov "+main_reigster_manager.hold_tmp()+", "+reg1, 1);
    }
    if (op == BINARY_OP_DIVIDE)
    {
        add_text("mov rax, "+ reg2, 1);
        add_text("mov rcx, "+reg1, 1);
        add_text("xor rdx, rdx", 1);
        add_text("div rcx ", 1);
        add_text("mov "+main_reigster_manager.hold_tmp()+", rax", 1);
    }
    if (op == BINARY_OP_EQUAL)
    {
        add_text("cmp "+reg1+", "+reg2, 1);
        add_text("sete al", 1);
        add_text("movzx eax, al", 1);
        add_text("mov "+main_reigster_manager.hold_tmp()+", rax", 1);
    }

    if (op == BINARY_OP_NEQUAL)
    {
        add_text("cmp "+reg1+", "+reg2, 1);
        add_text("setne al", 1);
        add_text("movzx eax, al", 1);
        add_text("mov "+main_reigster_manager.hold_tmp()+", rax", 1);
    }
}

void x86_64_symbol::skip_to_next_label_if_false(std::string to)
{
    add_text("cmp "+main_reigster_manager.hold_and_release_tmp()+", 0", 1);
    jmp_if_zero(to);
}


void x86_64_symbol::add_label(std::string name)
{
    add_text("."+name+":");
}
void x86_64_symbol::jump_to(std::string label)
{
    add_text("jmp ."+label, 1);
}
void x86_64_symbol::jmp_if_zero(std::string label)
{
    add_text("jz ."+label, 1);
}


std::string x86_64_symbol::next_label()
{
    return name+"L"+std::to_string(label_counter++);
}


std::string x86_64_symbol::get_text()
{
    return text_top + text_bottom;
}


void x86_64_asm::initialize()
{
    add_text("section .text");
    add_data("section .data");
    global_symbol.initialize("_start");
    global_symbol.exported = true;
    global_symbol.asm_raw_line("call main");
    global_symbol.asm_raw_line("mov "+main_reigster_manager.hold_and_release_tmp()+", rax");
    global_symbol.asm_raw_line("mov rdi, "+main_reigster_manager.hold_and_release_tmp()+"");
    global_symbol.asm_syscall(60); // We dont need to finalize because its directly exiting
    main_symbol.initialize("main");
    main_symbol.exported = false;
    current_context = &main_symbol;

}

void x86_64_asm::create_and_select_context(std::string name)
{

    if (name == "main")
    {
        this->global_variables = main_symbol.variables;
        isMain = true;
    }
    if (!isMain)
    {
        this->global_variables = main_symbol.variables;
    }
    x86_64_symbol current_contex;
    current_contex.initialize(name);
    symbols.push_back(current_contex);
    this->current_context = &symbols.back();
}

void x86_64_asm::finalize()
{
    if (!isMain)
    {
        main_symbol.finalize();
        symbols.push_back(main_symbol);
    }

}


void add_asm_line(std::string line, std::string *to, int indent)
{
    const std::string tabs(indent, '\t');
    const auto newline = tabs + line + "\n";
    *to += newline;
}

void x86_64_asm::add_text(std::string text, int indent)
{
    add_asm_line(text, &text_section, indent);
}

void x86_64_asm::add_data(std::string text, int indent)
{
    add_asm_line(text, &data_section, indent);
}

void x86_64_asm::add_global_variable(std::string name, std::string value)
{
    std::string val = name + ": \t";
    val += "dq " + value;
    add_data(val);
}


void x86_64_asm::print(bool save, std::string filename)
{
    std::string assembly;
    std::string symbolData;
    std::string symbolList = "global _start\n";
    //get all exported symbols
    for (auto symbol : symbols)
    {
        if (symbol.exported)
        {
            symbolList += "global" + symbol.name + "\n";
        }
        symbolData += symbol.get_text() + "\n";
    }
    assembly += data_section;
    assembly += "\n";
    assembly += text_section;
    assembly += symbolList;
    assembly += "\n";
    assembly += symbolData;
    assembly += global_symbol.get_text();
    printf("%s\n", assembly.c_str());

    if (save)
    {
        std::ofstream outfile(filename);
        if (!outfile)
        {
            fprintf(stderr, "Error: Cannot open file '%s' for writing\n", filename.c_str());
            return;
        }
        outfile << assembly;
        outfile.close();
    }
}


