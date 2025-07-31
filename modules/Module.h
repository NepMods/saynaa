//
// Created by arjun on 7/28/25.
//

#ifndef MODULES_HPP
#define MODULES_HPP
#include <string>

class ModuleContext
{
    public:

    void *source;
    void *input;
    void *output;
    void *meta;
    void *cmd_args;
    int argc;
    char** argv;


    int expexcted_next = -1;
    int from = -1;

    ModuleContext(void* source, void* input, void* output, void* meta, void* cmd_args, int argc, char** argv);
    void runNext(int priority, int from_priority);
};

class Module {
public:
    const char *name;
    int priority;
    int (*run)(ModuleContext *ctx);
    Module(const char* name, int priority, int (*run)(ModuleContext*));
};

void register_module(Module *module);
Module **get_registered_modules();

#ifndef MODULE_NAME
#define MODULE_NAME "dummy_module"
#endif

#ifndef MODULE_PRIORITY
#define MODULE_PRIORITY 0
#endif





#endif //MODULES_HPP
