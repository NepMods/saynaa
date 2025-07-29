#include "../../modules/Module.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

static int executor_run(ModuleContext *ctx) {

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

    std::vector<std::string> object_files;

    for (const auto& sa_file : all_sources) {
        std::string asm_file = sa_file.substr(0, sa_file.find_last_of('.')) + ".asm";
        std::string obj_file = sa_file.substr(0, sa_file.find_last_of('.')) + ".o";
        object_files.push_back(obj_file);

        std::stringstream nasm_cmd;
        nasm_cmd << "nasm -g -f elf64 " << asm_file;
        printf(">> %s\n", nasm_cmd.str().c_str());
        if (std::system(nasm_cmd.str().c_str()) != 0) {
            std::cerr << "NASM failed for: " << asm_file << "\n";
            return 1;
        }
    }

    std::stringstream ld_cmd;
    ld_cmd << "ld ";
    for (const auto& obj : object_files) {
        ld_cmd << obj << " ";
    }
    ld_cmd << "-o " << output;

    printf(">> %s\n", ld_cmd.str().c_str());
    if (std::system(ld_cmd.str().c_str()) != 0) {
        std::cerr << "Linking failed.\n";
        return 1;
    }

    printf("\n\033[1;32mGenerated executable: %s\033[0m\n", output.c_str());
    printf("\033[1;34mRun the executable with: ./%s\033[0m\n", output.c_str());
    printf("[execute] finished.\n");

    return 0;
}

static Module executor_mod = Module(MODULE_NAME, MODULE_PRIORITY, executor_run);

__attribute__((constructor))
void init_executor_module() {
    register_module(&executor_mod);
}
