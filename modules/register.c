#include "../modules.h"

#define MAX_MODULES 64
static CompilerModule *registered_modules[MAX_MODULES];
static int module_count = 0;

void register_module(CompilerModule *module) {
    if (module_count < MAX_MODULES) {
        registered_modules[module_count++] = module;
    }
}

CompilerModule **get_registered_modules() {
    return registered_modules;
}
