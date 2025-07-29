#include "../../modules/Module.h"
#include <cstdio>

#include "../../shared/value.h"
#include "AssemblyGen/AssemblyGen.h"


// you still use a normal C++ symbol for the run-function…
static int SVM_run(ModuleContext *ctx) {
    std::string* str = static_cast<std::string*>(ctx->meta);
    bool *linked = static_cast<bool*>(ctx->cmd_args);
    bool linked_real = *linked;

    Bytecode *code = (Bytecode *)ctx->input;
    Bytecode bytecode;

    bytecode = *code;
    AssemblyGen gen(bytecode);
    gen.generateAssembly(*str, linked_real);

    ctx->runNext(0, 999);
    delete static_cast<std::string*>(ctx->meta);

    return 0;
}

// …but at link/compile time, MODULE_NAME and MODULE_PRIORITY
// are injected from module.info via -D flags
static Module SVM_mod = Module(MODULE_NAME, MODULE_PRIORITY, SVM_run);

__attribute__((constructor))
void init_SVM_module() {
    register_module(&SVM_mod);
}
