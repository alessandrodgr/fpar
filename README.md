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
The library is a header-only library, but in order to download its dependency needs to be installed.
Thus after downloading the repository you can do the following:

```sh
mkdir build
cd build
../configure --no-bin --install-path=<install-path>
make install
```
All the header will be installed on `./<install-path>/include`.

### Dependencies
* a compiler for C++17
* OpenMP 4 or newer
* [immer](https://github.com/arximboldi/immer) library

## Usage
For the detailed documentation refer to the [wiki](../../wiki)

## Examples
Some examples are provided in the [tests](test) directory. They show both very basic programs and more complex ones, like matrix multiplication or sorting
