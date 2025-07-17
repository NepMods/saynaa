#include "../../modules.h"
#include "stdio.h"
#include <cstdio>
#include <cstdlib>

static int execute_run(CompilerContext *ctx)
{
    printf("[execute] running...\n");

    if (std::system("nasm -g -f elf64 saynaa.asm && ld saynaa.o -o app"))
      return 1;
    // print token name
    printf("\n\033[1;32mGenerated executable: app\033[0m");
    printf("\033[1;34m\nRun the executable with: ./app\033[0m\n");
    printf("[execute] finished.\n");

    return 0;
}

static CompilerModule execute_mod = {
   .name = "Execuatable generator",
   .priority = 3,
   .run = execute_run,
};

__attribute__((constructor))
void init_execute_module() {
   register_module(&execute_mod);
}
