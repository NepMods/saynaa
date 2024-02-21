/*
 * Copyright (c) 2023-2024 Mohammed Abdifatah. All rights reserved.
 * Distributed Under The MIT License
 *
 */

#ifndef saynaa_generator_h
#define saynaa_generator_h

#include "../compiler/compiler.h"
#include "../shared/common.h"
#include "../shared/value.h"

typedef enum {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} InterpretResult;

class Generator {
  enum {
    ADD_METHOD,
    SUB_METHOD,
    MUL_METHOD,
    DIV_METHOD,

    EQU_METHOD,
    NEQ_METHOD,
    UNK_METHOD,
  };
  typedef struct {
    std::stringstream *code;
  } test_weeye;

  InterpretResult run();
  void runtimeError(const std::string error);
  void push_label();
  auto checkVariable(std::string name);
  void BinaryOP(u_int32_t type);

  void store_Variable(const std::string &type, const std::string &val);
  void store_ptrAllVariable(int location);
  int total_allVariable = 0;

  void store_tmpValue();
  void get_tmpValue(const std::string &reg, std::stringstream *ss = nullptr, int *temp_value = nullptr);
  int total_tmpValue = 0;

  std::string assembly_filename;
  std::vector<stackVariable> stackVar;
  Bytecode *bytecode;

  std::stringstream tmp_label;

  // current_label: is a temporary variable
  std::stringstream current_label;
  std::stringstream *assembly_body;
  std::stringstream assembly_main;
  std::stringstream assembly_text; // SOON_REMOVING
  std::stringstream assembly_data;

  std::vector<std::string> assembly_label;
  std::vector<uint32_t> opcode;

public:
  Generator(std::string filename);
  void free();
  InterpretResult main(Bytecode &pBytecode);
};
#endif
