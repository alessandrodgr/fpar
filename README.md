# fpar
A data parallel implementation of [Backus FP](backus_turingaward_lecture.pdf) programming language as an embedded DSL in C++17

## Features
* All of FP functions and functional forms
* Possibility to evaluate some functions and functional forms in parallel
* Type-safe implementation of the polymorphic FP object type
* Extensible type system
* Immutable sequences
* Direct integration with C++ constructs, types, STL algorithms etc...
* Concise syntax

## Installation
Since this is a header-only library, it is sufficient to:
* download this repository in your project directory
* include the main header file
```cpp
#include "fpar/src/fpar.hpp"
```

### Dependencies
* a compiler for C++17
* OpenMP 4 or newer
* [immer](https://github.com/arximboldi/immer) library

## Examples
Some examples are provided in the [tests](src/test) directory. They show both very basic programs and more complex ones, like matrix multiplication or sorting
