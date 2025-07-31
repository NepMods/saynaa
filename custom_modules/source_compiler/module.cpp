#include "../../modules/Module.h"
#include <cstdio>
#include <iostream>
#include <vector>
#include "exports.h"

static std::vector<Source> sources;
static bool ran_once = false;
static int source_compiler_run(ModuleContext *ctx) {

    if (!ran_once)
    {
        std::vector<std::string> libs;
        std::string main_file;
        std::string output = "a.out";

        bool next_is_output = false;
        for (int i = 1; i < ctx->argc; ++i) {
            std::string arg = ctx->argv[i];

            if (next_is_output) {
                output = arg;
                next_is_output = false;
            } else if (arg == "-l") {
                if (i + 1 < ctx->argc) {
                    libs.push_back(ctx->argv[++i]);
                }
            } else if (arg == "-o") {
                next_is_output = true;
            } else if (arg[0] != '-') {
                main_file = arg;
            }
        }

        if (main_file.empty()) {
            std::cerr << "No main source file provided.\n";
            return 1;
        }


        std::vector<std::string> all_sources = libs;
        all_sources.push_back(main_file);

        for (auto source_file : all_sources)
        {
            bool isMain = source_file == main_file;
            sources.push_back({.name = source_file, .compiled = false ,.linked = !isMain});
        }

        ran_once = true;
        for (auto &source_file : sources)
        {
            if (!source_file.compiled)
            {
                std::printf("Compiling: %s %s\n", source_file.linked ? "linked->" : "main->", source_file.name.c_str());
                ctx->output = &source_file;
                ctx->runNext(1, 0);
                return 0;
            }
        }
    } else
    {
        for (auto &source_file : sources)
        {
            if (!source_file.compiled)
            {std::printf("Compiling: %s %s\n", source_file.linked ? "linked->" : "main->", source_file.name.c_str());

                ctx->output = &source_file;
                ctx->runNext(1, 0);
                return 0;
            }
        }
    }
    ctx->runNext(1000, 1);
    return 0;
}

static Module source_compiler_mod = Module(MODULE_NAME, MODULE_PRIORITY, source_compiler_run);
__attribute__((constructor))
void init_source_compiler_module() {
    register_module(&source_compiler_mod);
}

