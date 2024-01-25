#ifndef lang_utils_h
#define lang_utils_h

#include "common.h"

#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)

#define GROW_ARRAY(type, pointer, newCount) \
    (type*)reallocate(pointer, sizeof(type) * (newCount))

#define FREE_ARRAY(type, pointer) \
    reallocate(pointer, 0)


void* reallocate(void* pointer, size_t newSize);
#endif