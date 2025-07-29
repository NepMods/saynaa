#include "../../modules/Module.h"
#include <cstdio>
#include <string>

#include "../../shared/value.h"
#include "Parser/Parser.h"
#include "Tokenizer/Tokenizer.h"


Token *tokens;
Bytecode bytecode;

std::string opcodeToName(uint32_t opcode) {
    switch (opcode) {
    case OP_CONSTANT:     return "OP_CONSTANT";
    case OP_JUMP_HERE:    return "OP_JUMP_HERE";
    case OP_NULL:         return "OP_NULL";
    case OP_TRUE:         return "OP_TRUE";
    case OP_FALSE:        return "OP_FALSE";
    case OP_POP:          return "OP_POP";
    case OP_SET_LOCAL:    return "OP_SET_LOCAL";
    case OP_GET_LOCAL:    return "OP_GET_LOCAL";
    case OP_DEFINE_LOCAL: return "OP_DEFINE_LOCAL";
    case OP_EQUAL:        return "OP_EQUAL";
    case OP_NEQU:         return "OP_NEQU";
    case OP_GREATER:      return "OP_GREATER";
    case OP_LESS:         return "OP_LESS";
    case OP_ADD:          return "OP_ADD";
    case OP_SUBTRACT:     return "OP_SUBTRACT";
    case OP_MULTIPLY:     return "OP_MULTIPLY";
    case OP_DIVIDE:       return "OP_DIVIDE";
    case OP_NOT:          return "OP_NOT";
    case OP_NEGATE:       return "OP_NEGATE";
    case OP_BEG_FUNC:     return "OP_BEG_FUNC";
    case OP_END_FUNC:     return "OP_END_FUNC";
    case OP_CALL:         return "OP_CALL";
    case OP_JUMP_IF_NOT:  return "OP_JUMP_IF_NOT";
    case OP_JUMP:         return "OP_JUMP";
    case OP_RETURN:       return "OP_RETURN";
    case OP_DEF_PARAM: return "OP_DEF_PARAM";
    case OP_TEST:         return "OP_TEST";
    case OP_NONE:         return "OP_NONE";
    case OP_IMPORT: return  "OP_IMPORT";
    case OP_IMPORT_FROM: return "OP_IMPORT_FROM";
    default:
        return "UNKNOWN_" + std::to_string(opcode);
    }
}


void saveBytecodeToSmaliFile(const Bytecode &bytecode, const std::string &filename) {
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
        if (op == OP_CONSTANT || op == OP_DEFINE_LOCAL || op == OP_GET_LOCAL || op == OP_SET_LOCAL || op == OP_CALL || op == OP_BEG_FUNC || op == OP_DEF_PARAM || op == OP_JUMP_IF_NOT ||  op == OP_IMPORT || op == OP_IMPORT_FROM) {
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
                } else if ((op == OP_DEFINE_LOCAL || op == OP_GET_LOCAL || op == OP_SET_LOCAL || op == OP_BEG_FUNC ||  op == OP_IMPORT || op == OP_IMPORT_FROM) && operand < bytecode.name.size()) {
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

static int lexer_run(ModuleContext *ctx) {
    std::string* source_ptr = static_cast<std::string*>(ctx->source);
    if (!source_ptr) {
        fprintf(stderr, "[tokenizer] Error: Input is null.\n");
        return 1;
    }

    std::string& source = *source_ptr;
    std::string* str = static_cast<std::string*>(ctx->meta);

    bytecode = Bytecode();
    tokens = nullptr;

    Tokenizer tokenizer(source);
    tokenizer.scan(&tokens);

    Parser parser(tokens);
    parser.compile(&bytecode);
    ctx->output = &bytecode;
    saveBytecodeToSmaliFile(bytecode, (*str) + ".class");
    return 0;
}

// …but at link/compile time, MODULE_NAME and MODULE_PRIORITY
// are injected from module.info via -D flags
static Module lexer_mod = Module(MODULE_NAME, MODULE_PRIORITY, lexer_run);
__attribute__((constructor))
void init_lexer_module() {
    register_module(&lexer_mod);
}
