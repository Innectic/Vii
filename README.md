
# The Vii Programming Language

Vii is a new programming language aiming at fast compile time, with easy to write & read code.

Some of the goals are:
- K.I.S.S - Keep it simple, stupid.
- Huge projects shouldn't take forever to compile.
- Library management should be easy
- Multiple export backends: x64, LLVM, etc
- Easy to debug, no `printf` debugging should be needed
- Simple to contribute, and add features

# Contributing

Write good code if you want to contribute. Don't manage your own memory, if you can avoid it. If you can't avoid it, please just do it properly

If you want to know how things should be written, just look at how some of the code in the repo has been written.

# Compiling

To compile on linux, you need gcc 7.0 or higher.

```
git clone https://github.com/innectic/vii
cd vii
mkdir build
cmake ../src && make
```

Bam! Now you built it
