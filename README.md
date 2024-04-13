# Banana

This repository provides an exact solver - named Banana - to the one-sided
crossing minimization problem of a bipartite graph on two layers. This problem
involves arranging the nodes of one of the layers, aiming to minimize the
number of edge crossings. It was used as a submission to the 2024 Parameterized
Algorithms and Computational Experiments ([Pace 2023]
(https://pacechallenge.org/)). We approached this problem by implementing a
[Something](https://arxiv.org/abs/2110.06146) for it, combined with something
else.

A description of the our approach is available [here]().

## Build Application 

1. Create a build directory: `mkdir build && cd build`

2. Run cmake: `cmake ..`

3. Run make: `make`

## Run Tests
To run the tests, pass the name of a test file, as shown below (from build
folder):

`./pace < <test file>`

## Requirements
- A 64-bit Linux operating system.
- A compiler that supports C++17.
- The [cmake](www.cmake.org) build system.
