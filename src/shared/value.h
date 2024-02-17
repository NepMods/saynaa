/*
 * Copyright (c) 2023-2024 Mohammed Abdifatah. All rights reserved.
 * Distributed Under The MIT License
 *
 * src/shared/value.h
 *   contains: bytecode and stack variable
 */

#ifndef saynaa_public_h
#define saynaa_public_h

#include "common.h"

typedef enum {
  OP_CONSTANT,
  OP_NULL,
  OP_TRUE,
  OP_FALSE,
  OP_POP,
  OP_DEFINE_GLOBAL,
  OP_GET_GLOBAL,
  OP_EQUAL,
  OP_NEQU,
  OP_GREATER,
  OP_LESS,
  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_NOT,
  OP_NEGATE,
  OP_PRINT,
  OP_RETURN,
} OpCode;

typedef struct {
  std::vector<std::string> name;                     // variable name
  std::vector<std::variant<int, std::string>> value; // value can be str, int
  std::vector<int> lines;                            // line number of opcode
  std::vector<uint32_t> opcode;

  void free() {
    name.clear();
    value.clear();
    lines.clear();
    opcode.clear();
  }
} Bytecode;

typedef struct {
  std::string name;     // variable name
  int variableLocation; // variable location in assembly
} stackVariable;

#endif
