//
// Created by arjun on 7/21/25.
//

#ifndef ASSEMBLYGEN_H
#define ASSEMBLYGEN_H
#include "../../../shared/value.h"
#include "x86_64/x86_64_asm.h"


class AssemblyGen {
    Bytecode bytecode;
    x86_64_asm x86_64;


    int index = 0;
    uint32_t next_bytecode();
    uint32_t get_bytecode();
    uint32_t peek_bytecode(int offset);
    bool skip_to(uint32_t bytecode);
    void next();
    void parse_asm_fun();

public:
    AssemblyGen(Bytecode bytecode);
    std::string generateAssembly();
};



#endif //ASSEMBLYGEN_H
