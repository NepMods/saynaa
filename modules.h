// modules.h
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CompilerContext {
    void *source; 
    void *input;
    void *output;
    void *meta;
} CompilerContext;

typedef struct CompilerModule {
    const char *name;
    int priority;
    int (*run)(CompilerContext *ctx);
} CompilerModule;


// These functions must be accessible from C++
void register_module(CompilerModule *module);
CompilerModule **get_registered_modules();

#ifdef __cplusplus
}
#endif
