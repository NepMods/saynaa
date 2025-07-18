#include "../../modules.h"
#include <cstdio>

// you still use a normal C++ symbol for the run-function…
static int context_builder_run(CompilerContext *ctx) {
    ctx->output = ctx->input;
    return 0;
}

// …but at link/compile time, MODULE_NAME and MODULE_PRIORITY
// are injected from module.info via -D flags
static CompilerModule context_builder_mod = {
    .name     = MODULE_NAME,
    .priority = MODULE_PRIORITY,
    .run      = context_builder_run,
};

__attribute__((constructor))
void init_context_builder_module() {
    register_module(&context_builder_mod);
}
