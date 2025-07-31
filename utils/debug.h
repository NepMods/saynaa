/*
 * Copyright (c) 2023-2024 Mohammed Abdifatah. All rights reserved.
 * Distributed Under The MIT License
 *
 */

 #ifndef saynaa_debug_h
 #define saynaa_debug_h
 
 #include "../shared/common.h"
 #include "../shared/value.h"
 
 class Debug {
   int simpleInstruction(const std::string name, int offset);
   int callInstruction(const std::string name, int offset);
   int jumpInstruction(const std::string name, int offset);
   int constantInstructionName(const std::string name, int offset);
   int constantInstructionValue(const std::string name, int offset);
 
 public:
   Debug(Bytecode value);
   void disassemble(const std::string name);

   int disassembleInstruction(int offset);
 
   Bytecode bytecode;
 };
 #endif