// modular_compiler_base/main.cpp

#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "modules.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <source-file>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Failed to open file: " << argv[1] << "\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    std::string* source_ptr = new std::string(buffer.str());

    CompilerContext ctx;
    ctx.input = 0x0;
    ctx.output = nullptr;
    ctx.meta = nullptr;
    ctx.source = static_cast<void*>(source_ptr);


    CompilerModule** modules = get_registered_modules();

    // Count number of modules from null-terminated array
    size_t count = 0;
    while (modules[count] != nullptr) {
        ++count;
    }

    // Copy into vector for sorting
    std::vector<CompilerModule*> sorted_modules(modules, modules + count);

    // Sort modules by priority before running them
    std::sort(sorted_modules.begin(), sorted_modules.end(), [](CompilerModule* a, CompilerModule* b) {
        return a->priority < b->priority;
    });

    for (auto &mod : sorted_modules) {
        std::cout << "Running module: " << mod->name << std::endl;
        if (mod->run(&ctx) != 0) {
            std::cerr << "Module '" << mod->name << "' failed.\n";
            break;
        }
        ctx.input = ctx.output;
        ctx.output = nullptr;
    }

    return 0;
}
