#include "../generator/generator.h"
#include "../shared/common.h"

int main(int argc, const char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <input_file>\n";
    return 1;
  }
  std::string source = readFile(argv[1]);
  Parser parser(source);

  Bytecode bytecode;
  bool hadError = parser.compile(&bytecode);
  if (hadError) {
    std::cerr << "Compilling Failed!" << std::endl;
    return 1;
  }

  Generator gen("nasm.asm");
  gen.main(bytecode);
  gen.free();
  bytecode.free();

  std::puts(""); // new line
  if (std::system("nasm -f elf64 nasm.asm && ld nasm.o -o app"))
    return 1;
  return 0;
}