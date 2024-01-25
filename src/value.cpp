#include <stdio.h>

#include "utils.h"
#include "value.h"

ValueArray::ValueArray() : values(NULL), capacity(0), count(0) {
  
}

void ValueArray::write(Value value) {
  if (capacity < count + 1) {
    int oldCapacity = capacity;
    capacity = GROW_CAPACITY(oldCapacity);
    values = GROW_ARRAY(Value, values,
                               oldCapacity, capacity);
  }

  values[count] = value;
  count++;
}

bool ValueArray::valuesEqual(Value a, Value b) {
  if (a.type != b.type) return false;
  switch (a.type) {
    case VAL_BOOL:   return AS_BOOL(a) == AS_BOOL(b);
    case VAL_NIL:    return true;
    case VAL_NUMBER: return AS_NUMBER(a) == AS_NUMBER(b);
    default:         return false; // Unreachable.
  }
}

void ValueArray::printValue(Value value) {
  switch (value.type) {
    case VAL_BOOL:
      printf(AS_BOOL(value) ? "true" : "false");
      break;
    case VAL_NIL: printf("nil"); break;
    case VAL_NUMBER: printf("%g", AS_NUMBER(value)); break;
  }
}

void ValueArray::free() {
  FREE_ARRAY(Value, values, capacity);
  
  values = NULL;
  capacity = 0;
  count = 0;
}