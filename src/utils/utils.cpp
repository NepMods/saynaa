#include "utils.h"
#include <string.h>

std::string copyString(const char *chars, int length) {
  char *heapChars = new char[length + 1];
  memcpy(heapChars, chars, length);
  heapChars[length] = '\0';
  std::string result(heapChars);
  delete[] heapChars;
  return result;
}

std::string readFile(const char *path) {
  std::ifstream inputFile(path);
  if (!inputFile.is_open()) {
    std::cerr << "Error opening file: " << path << "\n";
    exit(1);
  }
  std::string source((std::istreambuf_iterator<char>(inputFile)),
                     std::istreambuf_iterator<char>());
  return source;
}

std::string generateRandomText(int length) {
  std::string characters =
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, characters.size() - 1);

  std::string randomText;
  for (int i = 0; i < length; ++i) {
    randomText += characters[dis(gen)];
  }

  return randomText;
}