#include "common.h"
#include "chunk.h"
#include "generator.h"

int main(int argc, const char* argv[]){
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }

    ifstream inputFile(argv[1]);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening file: " << argv[1] << "\n";
        return 1;
    }
    string source((istreambuf_iterator<char>(inputFile)),
                       istreambuf_iterator<char>());

    generator.run(source);
    generator.free();
    return 0;
}