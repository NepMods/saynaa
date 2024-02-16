# myCompiler

Welcome to myCompiler - written in c++ language, from scratch

## code look like this

```
let str = "result: ";
let x = 10;
let y = 20;
let z = x*y;

print(str);
print(z);
```

## Code Format

i am used clang-format inside vscode

## Variable is a dynamic?

I made an assembly variable named `allVariable` which I saved in the address of the array that holds all the global variables,
while each variable consists of **9 bytes**.
First byte: This is the variable's type
The other 8 bytes: if the variable is an integer, it holds the value of the variable, if it is a string, it holds the string of its address, while the string I saved in the `section .data`

## build

```
make
./saynaa example.sa && ./app
```

## License

Saynaa is available under the permissive MIT license.
