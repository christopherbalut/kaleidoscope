# Kaleidoscope

An implementation of the Kaleidoscope programming language in C++

## Status

Currently working on Chapter 1: Lexer.

## Requirements

- C++17 compiler
- CMake
- Ninja
- LLVM
- Clang

## Building

```bash
cmake -S . -B build -G Ninja \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_BUILD_TYPE=Debug

cmake --build build
```

## Running

```bash
./build/kaleidoscope
```
