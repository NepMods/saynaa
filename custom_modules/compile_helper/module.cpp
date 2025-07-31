#include "../../modules/Module.h"
#include <cstdio>

#include "../source_compiler/exports.h"
#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>

std::string strip_last_extension(const std::string& filename) {
    size_t last_dot = filename.rfind('.');
    if (last_dot == std::string::npos)
        return filename; // no extension
    return filename.substr(0, last_dot);
}
static int compile_helper_run(ModuleContext *ctx) {
    Source *source = static_cast<Source*>(ctx->input);
    if (source->compiled)
    {
        printf("Already compiled");
        return 1;
    }

    std::ifstream file(source->name);
    if (!file) {
        std::cerr << "Failed to open file: " << source->name << "\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string* source_ptr = new std::string(buffer.str());
    ctx->source = static_cast<void*>(source_ptr);
    source->compiled = true;

    ctx->meta = new std::string(strip_last_extension(source->name));
    ctx->cmd_args = new bool(source->linked);
    // printf("Output file: %s\n", ctx->out_name.c_str());
    ctx->runNext(2, 1);

    return 0;
}

static Module compile_helper_mod = Module(MODULE_NAME, MODULE_PRIORITY, compile_helper_run);
__attribute__((constructor))
void init_compile_helper_module() {
    register_module(&compile_helper_mod);
}

