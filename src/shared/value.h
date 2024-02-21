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
  OP_CONSTANT, //0
  OP_NULL, //1
  OP_TRUE, //2
  OP_FALSE, //3
  OP_POP, //4
  OP_DEFINE_GLOBAL, //5
  OP_GET_GLOBAL, //6
  OP_EQUAL, //7
  OP_NEQU, //8
  OP_GREATER, //9
  OP_LESS, //10
  OP_ADD, //11
  OP_SUBTRACT, //12
  OP_MULTIPLY, //13
  OP_DIVIDE, //14
  OP_NOT, //15
  OP_NEGATE, //16
  OP_BEG_FUNC, //17
  OP_END_FUNC, //18
  OP_CALL,  //19
  OP_PRINT, //20
  OP_RETURN, //21
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
