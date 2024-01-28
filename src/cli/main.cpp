#include "../shared/common.h"
#include "../generator/generator.h"

int main(int argc, const char* argv[]){
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }

    std::ifstream inputFile(argv[1]);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening file: " << argv[1] << "\n";
        return 1;
    }
    std::string source((std::istreambuf_iterator<char>(inputFile)),
                       std::istreambuf_iterator<char>());

    Generator gen;
    gen.main(source);
    gen.free();
    return 0;
}