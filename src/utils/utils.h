/*
 * Copyright (c) 2023-2024 Mohammed Abdifatah. All rights reserved.
 * Distributed Under The MIT License
 *
 */

#ifndef saynaa_utils_h
#define saynaa_utils_h

#include "../shared/common.h"

std::string copyString(const char *chars, int length);
std::string readFile(const char *path);
std::vector<int> stringToDecimal(const std::string &input);
std::string stringToHexDecimal(const std::string &value);
std::string generateRandomText(int length);

#endif