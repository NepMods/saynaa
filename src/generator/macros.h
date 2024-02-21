/*
 * Copyright (c) 2023-2024 Mohammed Abdifatah. All rights reserved.
 * Distributed Under The MIT License
 *
 */

#ifndef saynaa_macros_h
#define saynaa_macros_h

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