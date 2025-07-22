#include "../../modules.h"
#include <cstdio>

#include "../../shared/value.h"
#include "AssemblyGen/AssemblyGen.h"


// you still use a normal C++ symbol for the run-function…
static int SVM_run(CompilerContext *ctx) {
    Bytecode *code = (Bytecode *)ctx->input;
    Bytecode bytecode;

    bytecode = *code;
    AssemblyGen gen(bytecode);
    gen.generateAssembly();

    return 0;
}

// …but at link/compile time, MODULE_NAME and MODULE_PRIORITY
// are injected from module.info via -D flags
static CompilerModule SVM_mod = {
    .name     = MODULE_NAME,
    .priority = MODULE_PRIORITY,
    .run      = SVM_run,
};

__attribute__((constructor))
void init_SVM_module() {
    register_module(&SVM_mod);
}
