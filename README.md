# Banana

This repository provides an exact solver - named Banana - to the one-sided
crossing minimization problem of a bipartite graph on two layers. This problem
involves arranging the nodes of one of the layers, aiming to minimize the
number of edge crossings. It was used as a submission to the 2024 Parameterized
Algorithms and Computational Experiments
([Pace 2024](https://pacechallenge.org/)). We approached this problem by
implementing a [something]() for it, combined with [something else]().
Luis Higino is our director of compilation.

A description of the our approach will be available [here]().

## Build application

Simply run:

`mkdir build && cd build && cmake .. && make`

## Execution

### Default execution
To run a single test, redirect a file as input for the `pace` executable.

`./pace < <test file>`

### CMake rules (Higino)
To-Do

### Flags
We have implemented a series of flags that can be used to tweak the solver
for testing and implementation purposes. They are listed below.

#### Integer programming
- `ipsolver`: sets the solver that will be used to solve the integer program.
  At the moment, the available solvers are `lpsolve`.
- `ipformulation`: choose which formulation for the OSCM problem will be used.
  The possible values are `simple`, `shorter` and `quadratic`. Each formulation
  is described in the `ip_solver.cpp` file.

#### Verification
- `verify`: hello Higino.

## Submission

The submission is done through [Optil](https://www.optil.io/).

1. Clean the files that would conflict with Optil's compilation process. These
   files are listed in `.gitignore`, and, therefore, this can be accomplished
   using the `git clean -dfx` command.
2. After removing the files, run `tar -czvf pace.tgz ./*` to compile all the
   files.
3. Upload `pace.tgz` to Optil's submission page and choose `CMake` as the
   language.

## Requirements
- A 64-bit Linux operating system.
- A compiler that supports C++17.
- The [cmake](www.cmake.org) build system.
