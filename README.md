
# The Vii Programming Language

Vii is a next programming language aiming at fast compile speeds, with easy to write code.

Some of the goals are:
- Don't include random things no one will ever need
- Huge projects (5 million lines) shouldn't take a million years to compile.
- Library management should be easy
- The compiler should be influence-able, not just a static thing. (Meaning that you can ask the compiler to do things at compile time)
- Large amounts of optimizations
- Multiple export backends: x86, x64, LLVM, etc
- Easy to debug, no `printf` debugging should be needed
- Simple to contribute, and add features

# Contributing

Write good code if you want to contribute. *PLEASE* avoid managing memory yourself. No one is good at it, just use smart pointers for fuck sake.

If you want to know how things should be written, just look at how some of the code in the repo has been written.