//
// Created by arjun on 7/21/25.
//

#include "AssemblyGen.h"

#include "../../lexer/exports.h"
#include "x86_64/x86_64_asm.h"

AssemblyGen::AssemblyGen(Bytecode bytecode)
{
    this->bytecode = bytecode;
}

uint32_t AssemblyGen::get_bytecode()
{
    if (index < bytecode.opcode.size()) {
        return bytecode.opcode[index];
    }
    return OP_NONE;
}

uint32_t AssemblyGen::peek_bytecode(int offset)
{
    if (index + offset < bytecode.opcode.size()) {
        return bytecode.opcode[index + offset];
    }
    return OP_NONE;
}

uint32_t AssemblyGen::next_bytecode()
{

    printf("current bytecode: index: %d, %s to %s\n", index, opcodeToName(peek_bytecode(-1)).c_str(), opcodeToName(peek_bytecode(0)).c_str());

    index++;
    return get_bytecode();
}

std::string AssemblyGen::generateAssembly()
{
    x86_64.initialize();

    while (get_bytecode() != OP_NONE)
    {
        next();
    }
    x86_64.finalize();
    x86_64.print(true, "saynaa.asm");

    return "";
}

void AssemblyGen::next()
{
    int current_bytecode = get_bytecode();

    switch (current_bytecode)
    {
    case OP_BEG_FUNC:
        {
            next_bytecode();
            std::string name = bytecode.name.at(get_bytecode());
            x86_64.create_and_select_context(name);
            next_bytecode(); // skip name
        }
        break;

    case OP_CONSTANT:
        {
            auto value = bytecode.value[next_bytecode()];
            std::string val = std::holds_alternative<std::string>(value) ? std::get<std::string>(value): std::to_string(std::get<int>(value));
            if (!x86_64.isMain && x86_64.current_context->name == "main")
            {
                if (peek_bytecode(1) == OP_DEFINE_LOCAL)
                {
                    next_bytecode();
                    std::string name = bytecode.name.at(next_bytecode());
                    x86_64.add_global_variable(name, val);
                    x86_64.current_context->push_variable(name, x86_64.isMain);
                    next_bytecode();
                    return;
                }
            }
            x86_64.current_context->add_temp_var(val);
            next_bytecode();
        }
        break;
    case OP_NULL:

        {
            next_bytecode(); //skip
        };
        break;
    case OP_END_FUNC:
        {
            x86_64.current_context->finalize();
            next_bytecode();
        }
        break;

    case OP_RETURN:
        {
            x86_64.current_context->return_raw("mov rax, rbx");
            next_bytecode();
        }
        break;
    case OP_DEFINE_LOCAL:
        {
            std::string name = bytecode.name.at(next_bytecode());
            x86_64.current_context->push_variable(name, x86_64.isMain);
            next_bytecode();
        }
        break;
    case OP_GET_LOCAL:
        {
            std::string name = bytecode.name.at(next_bytecode());

            if (name == "asm")
            {
                parse_asm_fun();
                return;;
            }
            if (peek_bytecode(1) == OP_CALL || peek_bytecode(1) == OP_CONSTANT || peek_bytecode(1) == OP_GET_LOCAL)
            {
                int param_size = 0;
                while (next_bytecode() != OP_CALL)
                {
                    if (get_bytecode() == OP_CONSTANT)
                    {
                        auto value = bytecode.value[peek_bytecode(1)];
                        std::string val = std::holds_alternative<std::string>(value) ? std::get<std::string>(value): std::to_string(std::get<int>(value));
                        x86_64.current_context->push_call_parameter(val, param_size);
                        next_bytecode();
                    } else if (get_bytecode() == OP_GET_LOCAL)
                    {

                        std::string name_of = bytecode.name.at(next_bytecode());
                        x86_64.current_context->get_variable(name_of, x86_64.global_variables, x86_64.isMain);
                        x86_64.current_context->push_call_parameter("rbx", param_size);
                        // next_bytecode();
                    }
                    param_size++;
                }
                next_bytecode();
                x86_64.current_context->asm_call(name, param_size, x86_64.symbols);
            }
            else
            {
                if (name == "__temp__")
                {
                    next_bytecode();
                    return;
                }
                x86_64.current_context->get_variable(name, x86_64.global_variables, x86_64.isMain);
            }
            next_bytecode();
        }
        break;

    case OP_POP:
        next_bytecode();
        break;
    case OP_DEF_PARAM:
        auto name = bytecode.name.at(next_bytecode());
        x86_64.current_context->add_parameter(name, "0");
        next_bytecode();
        break;

    }


}

bool AssemblyGen::skip_to(uint32_t bytecode)
{
    while (get_bytecode() != bytecode)
    {
        index++;
    }
    return true;
}

void AssemblyGen::parse_asm_fun()
{
    next_bytecode();
    auto flagVal = bytecode.value[peek_bytecode(1)];
    int flag = 0;
    if (!std::holds_alternative<int>(flagVal))
    {
        throw std::runtime_error("Invalid flag value, it should be int");
    }
    flag = std::get<int>(flagVal);

    next_bytecode();
    next_bytecode();
    auto value = bytecode.value[peek_bytecode(1)];


    std::string val = std::holds_alternative<std::string>(value) ? std::get<std::string>(value): std::to_string(std::get<int>(value));
    x86_64.current_context->asm_raw_line(val);

    if (flag == 1)
    {
        x86_64.current_context->finalized = true;
    }

    skip_to(OP_TRUE);
    next_bytecode();
    next_bytecode();
    next_bytecode();

}


