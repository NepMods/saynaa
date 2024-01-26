#ifndef lang_debug_h
#define lang_debug_h

#include "common.h"
#include "public.h"

class Debug {
private:
    int disassembleInstruction(int offset);
    int simpleInstruction(const std::string name, int offset);
    int constantInstruction(const std::string name, int offset);
public:
    Debug(Value value);
    void disassemble(const std::string name);

    std::vector<int> value;
    std::vector<uint8_t> opcode;
    std::vector<int> lines;
};
#endif