// modular_compiler_base/main.cpp

#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "../modules/Module.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <source-file>\n";
        return 1;
    }

    // Create the ModuleContext with command-line arguments
    ModuleContext ctx(nullptr, nullptr, nullptr, nullptr, nullptr, argc, argv);

    // Load registered modules
    Module **modules = get_registered_modules();

    // Count number of modules in null-terminated array
    size_t count = 0;
    while (modules[count] != nullptr)
    {
        ++count;
    }

    std::vector<Module *> sorted_modules(modules, modules + count);

    // Sort modules by priority
    std::sort(sorted_modules.begin(), sorted_modules.end(), [](Module *a, Module *b)
              { return a->priority < b->priority; });

    int index = 0;

    while (index < sorted_modules.size())
    {
        Module *current_module = sorted_modules[index];

        ctx.from = current_module->priority;

        if (current_module->run(&ctx) != 0)
        {
            std::cerr << "Module '" << current_module->name << "' failed.\n";
            break;
        }

        ctx.input = ctx.output;
        ctx.output = nullptr;

        if (ctx.expexcted_next != -1)
        {
            // Jump to module with matching priority
            auto it = std::find_if(sorted_modules.begin(), sorted_modules.end(),
                                   [&](Module *mod)
                                   { return mod->priority == ctx.expexcted_next; });

            if (it != sorted_modules.end())
            {
                index = std::distance(sorted_modules.begin(), it);
                ctx.expexcted_next = -1; // Reset after jump
                continue;
            }
            else
            {
                std::cerr << "Expected next module with priority " << ctx.expexcted_next << " not found.\n";
                break;
            }
        }

        ++index;
    }

    return 0;
}
