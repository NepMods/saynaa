/*
 * Copyright (c) 2023-2024 Mohammed Abdifatah. All rights reserved.
 * Distributed Under The MIT License
 *
 */

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

std::vector<int> stringToDecimal(const std::string &input) {
  std::vector<int> decimalValues;
  for (size_t i = 0; i < input.size(); ++i) {
    if (input[i] == '\\' && i + 1 < input.size()) {
      switch (input[++i]) {
      case 'n':
        decimalValues.push_back(char('\n'));
        break;
      case 'r':
        decimalValues.push_back(char('\r'));
        break;
      case 't':
        decimalValues.push_back(char('\t'));
        break;
      case 'a':
        decimalValues.push_back(char('\a'));
        break;
      default:
        decimalValues.push_back(static_cast<int>(input[i]));
        break;
      }
    } else {
      decimalValues.push_back(static_cast<int>(input[i]));
    }
  }
  return decimalValues;
}

// convert string into hex decimal
std::string stringToHexDecimal(const std::string &value) {
  std::vector<int> decimalValues = stringToDecimal(value);
  std::stringstream output;
  for (int dval : decimalValues) {
    if (dval <= 16) {
      // 0-9, A, B, C, D, F
      output << "0x0" << std::uppercase << std::hex << dval << ", ";
    } else {
      output << "0x" << std::uppercase << std::hex << dval << ", ";
    }
  }
  output << "0x00 ; '" << value << "'\n";

  return output.str();
}

std::string generateRandomText(int length) {
  std::string characters =
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, characters.size() - 1);

  std::string randomText;
  for (int i = 0; i < length; ++i) {
    randomText += characters[dis(gen)];
  }

  return randomText;
}