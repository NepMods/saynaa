#include "utils.h"

std::string copyString(const char* chars, int length) {
    char* heapChars = (char *)realloc(NULL, sizeof(char) * (length + 1));
    memcpy(heapChars, chars, length);
    heapChars[length] = '\0';
    return heapChars;
}