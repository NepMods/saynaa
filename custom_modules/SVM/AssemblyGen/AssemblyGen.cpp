//
// Created by arjun on 7/21/25.
//

#include "AssemblyGen.h"

#include "../../lexer/exports.h"
#include "x86_64/x86_64_asm.h"

bool showLogs = true;

AssemblyGen::AssemblyGen(Bytecode bytecode)
{
    if (showLogs) printf(">> AssemblyGen::AssemblyGen()\n");
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
    // if (showLogs) printf(">> AssemblyGen::peek_bytecode(%d)\n", offset);
    if (index + offset < bytecode.opcode.size()) {
        return bytecode.opcode[index + offset];
    }
    return OP_NONE;
}

uint32_t AssemblyGen::next_bytecode()
{
    // if (showLogs) printf(">> AssemblyGen::next_bytecode()\n");

    index++;
    return get_bytecode();
}

std::string AssemblyGen::generateAssembly()
{
    if (showLogs) printf(">> AssemblyGen::generateAssembly()\n");

    x86_64.initialize();

    while (get_bytecode() != OP_NONE)
    {
        next();
    }
    x86_64.finalize();
    x86_64.print(true, "saynaa.asm");

    return "1";
}

void AssemblyGen::next()
{
    // if (showLogs) printf(">> AssemblyGen::next()\n");

    int current_bytecode = get_bytecode();
    printf("%d\n", current_bytecode);
    switch (current_bytecode)
    {
    case OP_BEG_FUNC:
        {
            if (showLogs) printf(">> Handling OP_BEG_FUNC\n");
            next_bytecode();
            std::string name = bytecode.name.at(get_bytecode());
            if (name != "main")
            {
                function_names.push_back(name);
            }
            x86_64.create_and_select_context(name);
            next_bytecode(); // skip name
        }
        break;
    case OP_NONE:
        next_bytecode();
        break;;
    case OP_TRUE:
    case OP_FALSE:
    case OP_NULL:
    case OP_CONSTANT:
        {
            if (showLogs) printf(">> Handling OP_CONSTANT\n");
            std::string val;
            if (get_bytecode() == OP_TRUE)
            {
                val = "1";
            } else if (get_bytecode() == OP_FALSE || get_bytecode() == OP_NULL)
            {
                val = "0";
            } else
            {

                auto value = bytecode.value[peek_bytecode(1)];
                val = std::holds_alternative<std::string>(value) ? std::get<std::string>(value): std::to_string(std::get<int>(value));
            }
            next_bytecode();
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
    case OP_END_FUNC:
        {
            if (showLogs) printf(">> Handling OP_END_FUNC\n");
            x86_64.current_context->finalize();
            next_bytecode();
        }
        break;

    case OP_RETURN:
        {
            if (showLogs) printf(">> Handling OP_RETURN\n");
            x86_64.current_context->return_l();
            next_bytecode();
        }
        break;
    case OP_DEFINE_LOCAL:
        {
            if (showLogs) printf(">> Handling OP_DEFINE_LOCAL\n");
            std::string name = bytecode.name.at(next_bytecode());
            x86_64.current_context->push_variable(name, x86_64.isMain);
            next_bytecode();
        }
        break;
    case OP_SET_LOCAL:
        {
            if (showLogs) printf(">> Handling OP_SET_LOCAL\n");
            std::string name = bytecode.name.at(next_bytecode());
            x86_64.current_context->set_variable(name, "0", x86_64.global_variables, x86_64.isMain);
            next_bytecode();
        }
        break;
    case OP_GET_LOCAL:
        {
            if (showLogs) printf(">> Handling OP_GET_LOCAL\n");

            std::string name = bytecode.name.at(next_bytecode());

            if (name == "asm")
            {
                parse_asm_fun();
                return;
            }auto it = std::find(function_names.begin(), function_names.end(), name);
            if (it != function_names.end())
            {
                int param_size = 0;
                while (next_bytecode() != OP_CALL)
                {
                    // printf("A Function %d", peek_bytecode(1) == OP_CALL);

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
        if (showLogs) printf(">> Handling OP_POP\n");
        next_bytecode();
        break;
    case OP_DEF_PARAM:
        {
            if (showLogs) printf(">> Handling OP_DEF_PARAM\n");
            auto name = bytecode.name.at(next_bytecode());
            x86_64.current_context->add_parameter(name, "0");
            next_bytecode();
        }
        break;
    case OP_ADD:
        {
            if (showLogs) printf(">> Handling OP_ADD\n");
            x86_64.current_context->binary_op(BINARY_OP_ADD);
            next_bytecode();
        }
        break;

    case OP_SUBTRACT:
        {
            if (showLogs) printf(">> Handling OP_SUBTRACT\n");
            x86_64.current_context->binary_op(BINARY_OP_SUBTRACT);
            next_bytecode();

        }
        break;

    case OP_MULTIPLY:
        {
            if (showLogs) printf(">> Handling OP_MULTIPLY\n");
            x86_64.current_context->binary_op(BINARY_OP_MULTIPLY);
            next_bytecode();

        }
        break;

    case OP_DIVIDE:
        {
            if (showLogs) printf(">> Handling OP_DIVIDE\n");
            x86_64.current_context->binary_op(BINARY_OP_DIVIDE);
            next_bytecode();

        }
        break;


    case OP_EQUAL:
        {
            // if (showLogs) printf(">> Handling OP_EQUAL\n");
            x86_64.current_context->binary_op(BINARY_OP_EQUAL);
            next_bytecode();
        }
        break;
    case OP_NEQU:
        {
            if (showLogs) printf(">> Handling OP_NEQU\n");
            x86_64.current_context->binary_op(BINARY_OP_NEQUAL);
            next_bytecode();

        }break;;
    case OP_JUMP_IF_NOT:
        {
            int goto_path = next_bytecode();
            auto next_label_name = x86_64.current_context->next_label();
            condition_label(goto_path, next_label_name);
            x86_64.current_context->skip_to_next_label_if_false(next_label_name);
            next_bytecode();
         }
        break;

    case OP_JUMP:
        {

            int goto_path = next_bytecode();
            auto next_label_name = x86_64.current_context->next_label();
            condition_label(goto_path, next_label_name);
            x86_64.current_context->jump_to(next_label_name);
            next_bytecode();
        }break;
    case OP_JUMP_HERE: {
            std::string goto_path = std::get<std::string>(bytecode.value[next_bytecode()]);
            x86_64.current_context->add_label(goto_path);
            next_bytecode();
    }
    break;
    }

}

bool AssemblyGen::skip_to(uint32_t bytecode)
{
    if (showLogs) printf(">> AssemblyGen::skip_to(%u)\n", bytecode);
    while (get_bytecode() != bytecode)
    {
        index++;
    }
    return true;
}



void saveBytecodeToSmaliFile2(const Bytecode &bytecode, const std::string &filename) {
    std::ofstream out(filename);
    if (!out.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << "\n";
        return;
    }

    out << "# Bytecode dump (.smali-like)\n\n";

    // Opcodes section

    out << ".code\n";
    for (size_t i = 0; i < bytecode.opcode.size();) {
        uint32_t op = bytecode.opcode[i];
        uint32_t line = (i < bytecode.lines.size()) ? bytecode.lines[i] : 0;
        std::string name = opcodeToName(op);

        out << "    " << i << ": " << name << " (" << op << ")";
        if (line > 0) out << "   # line " << line;

        // Example: opcodes with 1 operand:
        if (op == OP_CONSTANT || op == OP_DEFINE_LOCAL || op == OP_GET_LOCAL || op == OP_SET_LOCAL || op == OP_CALL || op == OP_BEG_FUNC || op == OP_DEF_PARAM || op == OP_JUMP_IF_NOT || op == OP_JUMP_HERE || op == OP_JUMP) {
            if (i + 1 < bytecode.opcode.size()) {
                uint32_t operand = bytecode.opcode[i + 1];
                out << " " << operand;

                // Annotate constants or names
                if (op == OP_CONSTANT && operand < bytecode.value.size()) {
                    const auto &v = bytecode.value[operand];
                    out << " (";
                    if (std::holds_alternative<int>(v)) {
                        out << std::get<int>(v);
                    } else if (std::holds_alternative<std::string>(v)) {
                        out << "\"" << std::get<std::string>(v) << "\"";
                    }
                    out << ")";
                } else if ((op == OP_DEFINE_LOCAL || op == OP_GET_LOCAL || op == OP_SET_LOCAL || op == OP_BEG_FUNC) && operand < bytecode.name.size()) {
                    out << " (" << bytecode.name[operand] << ")";
                }
                i += 2;
            } else {
                i++;
            }
        } else {
            i++;
        }
        out << "\n";
    }

    // Constants section
    out << ".constants\n";
    for (size_t i = 0; i < bytecode.value.size(); ++i) {
        out << "    const " << i << ": ";
        const auto &v = bytecode.value[i];
        if (std::holds_alternative<int>(v)) {
            out << std::get<int>(v);
        } else if (std::holds_alternative<std::string>(v)) {
            out << "\"" << std::get<std::string>(v) << "\"";
        }
        out << "\n";
    }
    out << "\n";

    // Names section
    out << ".names\n";
    for (size_t i = 0; i < bytecode.name.size(); ++i) {
        out << "    name " << i << ": " << bytecode.name[i] << "\n";
    }

    out.close();

    std::cout << "Bytecode saved to " << filename << std::endl;
}


void AssemblyGen::condition_label(int indx, std::string label)
{

    bytecode.value.push_back(label);
    uint32_t constant = bytecode.value.size() - 1;

    bytecode.opcode.insert(bytecode.opcode.begin() + indx, OP_JUMP_HERE);
    bytecode.lines.insert(bytecode.lines.begin() + indx, 22);

    bytecode.opcode.insert(bytecode.opcode.begin() + indx + 1, constant);
    bytecode.lines.insert(bytecode.lines.begin() + indx + 1, 23);
    saveBytecodeToSmaliFile2(bytecode, "main_r.class");
}


void AssemblyGen::parse_asm_fun()
{
    if (showLogs) printf(">> AssemblyGen::parse_asm_fun()\n");
    next_bytecode(); // skip the OP_GET_LOCAL

    auto flagVal = bytecode.value[next_bytecode()]; // skip the OP_CONSTANT and get flag value
    int flag = 0;
    if (!std::holds_alternative<int>(flagVal))
    {
        throw std::runtime_error("Invalid flag value, it should be int");
    }
    flag = std::get<int>(flagVal);

    next_bytecode(); // skip the flag value

    auto value = bytecode.value[next_bytecode()]; // skip OP_CONSTANT and get asm value

    std::string val = std::holds_alternative<std::string>(value) ? std::get<std::string>(value): std::to_string(std::get<int>(value));
    x86_64.current_context->asm_raw_line(val);

    if (flag == 1)
    {
        x86_64.current_context->finalized = true;
    }
    next_bytecode(); // skip the value
    next_bytecode(); // skip OP_CALL
    next_bytecode(); // skip OP_POP
}
