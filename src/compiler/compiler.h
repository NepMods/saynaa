#ifndef lang_compiler_h
#define lang_compiler_h

#include "../shared/common.h"
#include "../shared/value.h"
#include "../utils/utils.h"
#include "../utils/debug.h"

bool compile(const std::string source, Value* value);

#endif