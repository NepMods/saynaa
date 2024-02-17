<p align="center"> <img src="docs/assets/icon.png" alt="Saynaa-Language" title="Saynaa Programming Language" align="center"> </p>

# Saynaa Compiler Language

Welcome to saynaa - written in c++ language, from scratch

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

## how the variable is converted into assembly?

I have created an assembly variable named `allVariable`, which I have stored at the address of an array holding all the global variables. Each variable within this array consists of **9 bytes**:

- The first byte denotes the variable's type.
- The remaining 8 bytes store the variable's value. For integer variables, this space holds the value itself. For string variables, it contains the address of the string, which is stored in the section .data.

and also created another variable called `tmpValue`, which temporarily stores the address of variable
like when I want to use binary expression, and etc

## build

```
make
./saynaa example.sa && ./app
```

## License

Saynaa is available under the permissive MIT license.
