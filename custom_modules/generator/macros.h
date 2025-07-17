/*
 * Copyright (c) 2023-2024 Mohammed Abdifatah. All rights reserved.
 * Distributed Under The MIT License
 *
 */

 #ifndef saynaa_macros_h
 #define saynaa_macros_h
 
 #define bss_section       \
   "section .bss\n"        \
   "    struc Variable\n"  \
   "      vType resb 1\n"  \
   "      vSize resd 1\n"  \
   "      vPtr  resq 1\n"  \
   "    endstruc\n\n"      \
 
 #define data_section           \
   "section .data\n"            \
   "    allVariable: dq 0\n"    \
   "    tmpValue: dq 0\n"       \
   "    exit_code: dq 0\n"      \
   "    TYPE_INT: equ 20\n"     \
   "    TYPE_STR: equ 10\n\n"   \
 
 
 // begin assembly code each label
 #define beg_label                                                              \
   "    push rbp\n"                                                             \
   "    mov rbp, rsp\n"                                                         \
   "    ; natural code\n";
 
 // end assembly code each label
 #define end_label                                                              \
   "    pop rbp\n"                                                              \
   "    ret\n";
 
 #endif