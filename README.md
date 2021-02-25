<!-- Copyright (c) 2021 Christopher Taylor                                          -->
<!-- Copyright (c) 2020 Christopher Taylor                                          -->
<!--                                                                                -->
<!--   Distributed under the Boost Software License, Version 1.0. (See accompanying -->
<!--   file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        -->

# Scrap Your Boilerplate Plus Plus (sybpp)

Project details [here](http://www.github.com/ct-clmsn/sybpp/).

This project implements basic functionality from `Scrap Your Boilerplate: A Practical Design Pattern for Generic Programming`, Lammel, Peyton-Jones, 2003.
The paper can be found [here](https://www.microsoft.com/en-us/research/wp-content/uploads/2003/01/hmap.pdf).

`Scrap Your Boilerplate` creates generic traversals over data types. Users identify a target type and provide a function that accepts
the identified type as an input parameter. `sybpp` computes, at compile-time, the generic traversal code. This removes the need for
users to implement "boilerplate" data structure traversal code (a visitor pattern) in order apply a function.

### Notes

Users are required to wrap data types `sybpp` encounters with the `BOOST_FUSION_ADAPT_STRUCT` macro. Review the `demo.cpp` file for
code examples.

### License

Boost Software License 1.0

### Features

* everywhere
* one-layer traversal
* recursive traversal
* queries
* transform
* functions, functors, std::bind, lambdas supported
* STL container types
* supports std::variant, std::tuple, std::pair, std::optional

### Special Thanks

Hartmut Kaiser for suggesting this paper.

### Author

Christopher Taylor

### Dependencies

* C++17
* Boost.Fusion
