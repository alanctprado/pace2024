# Dino

This repository provides an exact solver to the twin-width of a graph, named Dino. It was used as a submission to the 2023
Parameterized Algorithms and Computational Experiments ([Pace 2023](https://pacechallenge.org/)). We approached this problem by implementing the [SAT formulation](https://arxiv.org/abs/2110.06146) for it, combined with an algorithm which finds a modular-like decomposition of a graph and an algorithm to find the twin-width of trees.

A description of the our approach is available [here](description.pdf).

## Build Application 

1. Create a build directory: `mkdir build && cd build`

2. Run cmake: `cmake ..`

3. Run make: `make`

## Run Tests
To run the tests, pass the name of a test file, as shown below (from build folder):

`./pace < <test file>`

## Requirements
- A 64-bit Linux operating system.
- A compiler that supports C++17.
- The [cmake](www.cmake.org) build system.
