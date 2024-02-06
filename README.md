# myCompiler
Welcome to myCompiler - written in c++ language, from scratch

## code look like this
```
let str = "result: ";

print(str);
print(10*20);
```

## Code Format
i am used clang-format inside vscode

## build
```
make
./lang 1.lang
nasm -f elf64 nasm.asm && ld nasm.o -o app && ./app
```
