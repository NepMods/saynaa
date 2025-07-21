//
// Created by arjun on 7/21/25.
//

#ifndef X86_64_ASM_H
#define X86_64_ASM_H
#include <string>
#include <vector>

void add_asm_line(std::string line, std::string *to, int indent = 0);

enum  x86_var_type
{
    STRING,
    NUMBER
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
    void initialize(std::string name);
    void finalize();
    void runtimeError(std::string text);

    void asm_call(std::string name);
    void asm_raw_line(std::string data);
    void asm_syscall(int syscall);
    void return_raw(std::string data);
    void add_temp_var(std::string value);
    void push_variable(std::string name, bool isMain);
    void get_variable(std::string name, std::vector<x86_64_variable> &g_variables, bool isMain);
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

    bool isMain = false;

    void initialize();
    void finalize();
    void create_and_select_context(std::string name);
    void add_global_variable(std::string name, std::string value);
    void print(bool save, std::string filename);

};



#endif //X86_64_ASM_H
