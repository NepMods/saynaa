#ifndef lang_utils_h
#define lang_utils_h

#include "../shared/common.h"

std::string copyString(const char *chars, int length);
std::string readFile(const char *path);
std::string generateRandomText(int length);

#endif