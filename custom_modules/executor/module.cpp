#include "../../modules.h"
#include <cstdio>
#include "stdio.h"
#include <cstdio>
#include <cstdlib>

// you still use a normal C++ symbol for the run-function…
static int executor_run(CompilerContext *ctx) {

    printf("[execute] running...\n");

    if (std::system("nasm -g -f elf64 saynaa.asm && ld saynaa.o -o app"))
        return 1;
    // print token name
    printf("\n\033[1;32mGenerated executable: app\033[0m");
    printf("\033[1;34m\nRun the executable with: ./app\033[0m\n");
    printf("[ execute] finished.\n");

    return 0;
}

// …but at link/compile time, MODULE_NAME and MODULE_PRIORITY
// are injected from module.info via -D flags
static CompilerModule executor_mod = {
    .name     = MODULE_NAME,
    .priority = MODULE_PRIORITY,
    .run      = executor_run,
};

__attribute__((constructor))
void init_executor_module() {
    register_module(&executor_mod);
}
