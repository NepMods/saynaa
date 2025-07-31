<p align="center"> <img src="https://raw.githubusercontent.com/NepMods/saynaa/bbc0d121f25ff05b7f752deeaaa7abb8fad67d3d/docs/assets/icon.png" alt="Saynaa-Language" title="Saynaa Programming Language" align="center"> </p>

# Saynaa Compiler Language

Welcome to **Saynaa** – a programming language written in C++ from scratch.

---

## Run Example


To run the example code provided, use following command

``make``

``./compiler -l stdlib/print_int.sa examples/main.sa -o examples/app``

this should output output as expected

# Note: 
```
Now it supports import and export functions
you can exporta funcion like:

// sum.sa
export function sum(a, b) {
...
}

and import like

//main.sa
import sum from "sum"

function main() {
  return sum(2,3);
}


and you build it like:

./compiler -l sum.sa main.sa -o app

you can add many -l flags
for example

./compiler -l sum.sa -l stdlib/print_int.sa main.sa -o app

WARNING: while iporting, the name of file
for example its "sum" here because we have -l sum.sa

import sum from "sum"

if we had -l example/main.sa
we would do

import sum from "example/main"


the import filename is always as defined to the compiler command, not relative file path
we can fix it later

```



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
