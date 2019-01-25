# fpar
A data parallel implementation of [Backus FP](backus_turingaward_lecture.pdf) programming language as an embedded DSL in C++17

## Features
* Type-safe implementation of FP polymorphic object type
* Extensible type system
* Immutable sequences
* All of FP functions and functional forms
* Possibility to evaluate some functions and functional forms in parallel
* Direct integration with C++ constructs, types, STL algorithms etc...
* Concise syntax

## Installation
Since this is a header-only library, it is sufficient to:
* download this repository in your project directory
* include the main header file
```
#include "fpar/src/fpar.hpp"
```

## Usage

## Examples
Further examples are provided in the [tests](src/test) directory. They show both very basic programs and more complex ones, like matrix multiplication or sorting
