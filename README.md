<p align="center"> <img src="https://raw.githubusercontent.com/NepMods/saynaa/bbc0d121f25ff05b7f752deeaaa7abb8fad67d3d/docs/assets/icon.png" alt="Saynaa-Language" title="Saynaa Programming Language" align="center"> </p>

# Saynaa Compiler Language

Welcome to **Saynaa** – a programming language written in C++ from scratch.

---

## Code Example

```js
let addWithFive = 5;

function sum(a, b) {
  asm(0, `
        mov rbx, rdi
        add rbx, rsi
    `);
  return __temp__;
}

function main() {
  return sum(addWithFive, 30);
}
```

## Functions

```js
function print_int(val) {
  asm(0, `; allocate 32 bytes on stack for buffer (enough for 20 digits + newline)
                sub rsp, 32
                lea rsi, [rsp + 31]    ; point to end of buffer
                mov byte [rsi], 10     ; newline
                dec rsi

                mov rax, rdi           ; value to convert
                mov rcx, 10

            .convert_loop:
                xor rdx, rdx
                div rcx                ; divide rax by 10 -> quotient in rax, remainder in rdx
                add dl, '0'
                mov [rsi], dl
                dec rsi
                test rax, rax
                jnz .convert_loop

                inc rsi                ; point to first digit

                ; write syscall
                mov rax, 1             ; syscall: write
                mov rdi, 1             ; stdout
                mov rdx, rsp
                lea rdx, [rsp + 32]    ; end of stack buffer
                sub rdx, rsi           ; length = end - start
                mov rsi, rsi           ; buffer pointer is already in rsi
                syscall

                ; restore stack
                add rsp, 32`);
}

function main() {
  return print_int(3000);
}
```

## Main Function

```js
function main() {
  print("Inside Main function");
  return 0;
}

// Output: Inside Main function
```


## Inline Assembly

```asm(1, "ASSEMBLY_CODE"); //for ignoring everyting after asm func```

```asm(0, "ASSEMBLY_CODE"); //for keeping everyting after asm func```
```js
function value() {
  let retVal = "HI\n";
  __system_tmpvalue_add(1); // We are accessing a var in asm, that needs one extra temp stack to be increased
  asm(1, "    mov rax, qword[allVariable]
  mov rbx, qword[rax+0]
  mov rax, rbx
  mov rbx, qword[tmpValue]
  mov qword[rbx+0], rax
  mov rax, rbx");

}

print(value());
// Output: HI
```

RETURNS A VALUE FROM ASM

---

## Code Format

I use `clang-format` inside VSCode for formatting.

---

## How Are Variables Converted Into Assembly?

Saynaa generates assembly code and manages variables using:

- A global array called `allVariable`
  - Each entry is **9 bytes**:
    - 1 byte for type
    - 8 bytes for value (or pointer to string in `.data`)
- A temporary variable `tmpValue` is used for intermediate storage like binary operations, etc.

---

## Modular Architecture

Saynaa is designed to be **fully modular**, supporting loadable modules with priorities. Modules are placed inside the `custom_modules` directory.

### 📦 Example Module (Execute)

Path: `custom_modules/execute/module.cpp`

```cpp
#include "../../modules.h"
#include "stdio.h"
#include <cstdio>
#include <cstdlib>

static int execute_run(CompilerContext *ctx)
{
    printf("[execute] running...\n");

    if (std::system("nasm -g -f elf64 saynaa.asm && ld saynaa.o -o app"))
      return 1;

    printf("\n\033[1;32mGenerated executable: app\033[0m");
    printf("\033[1;34m\nRun the executable with: ./app\033[0m\n");
    printf("[execute] finished.\n");

    return 0;
}

static CompilerModule execute_mod = {
   .name = "Execuatable generator",
   .priority = 3,
   .run = execute_run,
};

__attribute__((constructor))
void init_execute_module() {
   register_module(&execute_mod);
}
```

---

## 🧩 How to Add a Custom Module
```shell

make module NAME=custom_module_name
```


## Later you can add dependency to other module, change priority ad module.info file

4. **Rebuild** the compiler:

```bash
make
```

5. **Run** the compiler and check output:

```bash
./saynaa example.sa
```

---

## Project Structure

```
.
├── app
├── count-line.sh
├── custom_modules
│   ├── compiler
│   │   ├── DEPENDENCY
│   │   ├── exports.h
│   │   └── module.cpp
│   ├── execute
│   │   ├── DEPENDENCY
│   │   └── module.cpp
│   ├── generator
│   │   ├── DEPENDENCY
│   │   ├── generator.h
│   │   ├── macros.h
│   │   └── module.cpp
│   └── tokenizer
│       ├── exports.h
│       └── module.cpp
├── example.sa
├── main.cpp
├── main.sa
├── Makefile
├── modules
│   └── register.c
├── modules.h
├── README.md
├── saynaa.asm
├── shared
│   ├── common.h
│   └── value.h
├── test.sa
└── utils
    ├── debug.cpp
    ├── debug.h
    ├── utils.cpp
    └── utils.h
```

---

## Build

```bash
make
./saynaa example.sa && ./app
```

---

## License

Saynaa is available under the permissive **MIT license**.
