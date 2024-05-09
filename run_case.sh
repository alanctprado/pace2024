#!/usr/bin/bash

if [ $# -lt 2 ]; then
    echo "Usage: $0 <set> <case> <args>"
    exit 1
fi

# Run test case, the solution is expected to be verified inside the solver itself
echo "running for case $1/$2:"
time ./build/pace --verify="./test/$1/solutions/$2.sol" "$@" < "./test/$1/instances/$2.gr" > /dev/null
