//
// Created by arjun on 7/28/25.
//

#include "Module.h"
#define MAX_MODULES 64
static Module *registered_modules[MAX_MODULES];
static int module_count = 0;

ModuleContext::ModuleContext(void* source, void* input, void* output, void* meta, void* cmd_args, int argc, char** argv) : source(source), input(input), output(output), meta(meta), cmd_args(cmd_args), argc(argc), argv(argv) {}
Module::Module(const char* name, int priority, int (*run)(ModuleContext*)):  name(name), priority(priority), run(run) {}

void ModuleContext::runNext(int priority, int from_priority)
{
    expexcted_next = priority;
    from = from_priority;
}



void register_module(Module *module) {
    if (module_count < MAX_MODULES) {
        registered_modules[module_count++] = module;
    }
}

Module **get_registered_modules() {
    return registered_modules;
}