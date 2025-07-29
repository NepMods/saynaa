//
// Created by arjun on 7/21/25.
//

#ifndef X86_64_ASM_H
#define X86_64_ASM_H
#include <string>
#include <vector>

void add_asm_line(std::string line, std::string *to, int indent = 0);

bool has_export(const std::string& file_name, const std::string& func_name);

class x86_64_register_manager {
    std::vector<std::string> temp_registers = {"rbx", "rcx", "rdx"};
    int held = -1;

public:
    std::string hold_tmp();
    x86_64_register_manager() : held(-1) {}
    std::string release_tmp(int i = 1);
    std::string hold_and_release_tmp();
    std::string pull_tmp();
};

enum  x86_var_type
{
    STRING,
    NUMBER
};

enum BINARY_OP
{
    BINARY_OP_ADD,
    BINARY_OP_SUBTRACT,
    BINARY_OP_MULTIPLY,
    BINARY_OP_DIVIDE,
    BINARY_OP_EQUAL,
    BINARY_OP_NEQUAL
};

struct x86_64_variable
{
    x86_var_type type;
    std::string name;
    int base_pointer;
};

struct x86_64_symbol
{
private:
    void add_text(std::string text, int indent = 0);
    void add_text_top(std::string text, int indent = 0);

public:
    std::string name;
    std::string text_top;
    std::string text_bottom;
    bool exported = false;
    bool returned = false;
    bool finalized = false;
    int current_base_pointer = 0;
    std::vector<x86_64_variable> variables;

    int parameter_size = 0;
    std::vector<x86_64_variable> parameters;
    int required_parameters = 0;
    bool is_param_vargs = false;
    std::string exit_label;
    int label_counter = 0;

    void initialize(std::string name);
    void finalize();
    void runtimeError(std::string text);

    void asm_call(std::string name, int param_size, std::vector<x86_64_symbol> list_symbols, bool imported = false);
    void asm_raw_line(std::string data);
    void asm_syscall(int syscall);
    void return_l();
    void add_temp_var(std::string value);
    void push_variable(std::string name, bool isMain);
    void get_variable(std::string name, std::vector<x86_64_variable> &g_variables, bool isMain);
    void add_parameter(std::string name, std::string def_value);
    void push_call_parameter(std::string value, int size);
    void set_variable(std::string name, std::string value, std::vector<x86_64_variable> &g_variables, bool isMain);
    void binary_op(BINARY_OP op);
    void add_label(std::string name);
    void skip_to_next_label_if_false(std::string to);
    std::string next_label();
    void jump_to(std::string label);
    void jmp_if_zero(std::string label);
    std::string get_text();


};

struct x86_64_asm {

private:
    void add_text(std::string text, int indent = 0);
    void add_data(std::string text, int indent = 0);
public:
    std::string text_section;
    std::string data_section;
    x86_64_symbol global_symbol;
    x86_64_symbol main_symbol;
    x86_64_symbol *current_context;
    std::vector<x86_64_symbol> symbols;
    std::vector<x86_64_variable> global_variables;
    std::string file_name;
    std::vector<std::string> imports;

    bool isMain = false;
    bool linked = false;

    void initialize(bool linked);
    void finalize();
    void create_and_select_context(std::string name, bool exported = false);
    void add_global_variable(std::string name, std::string value);
    void print(bool save, std::string filename);
    void import_from(std::string filename, std::string name);
    x86_64_symbol *get_current_context();

};



#endif //X86_64_ASM_H
