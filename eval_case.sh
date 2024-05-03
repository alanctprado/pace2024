#!/bin/bash

if [ $# -lt 4 ]; then
    echo "Usage: $0 <set> <case> <tl> <ml> <args>"
    exit 1
fi

ARGS=$*

# Run test case, the solution is expected to be verified inside the solver itself
echo "running for case $1/$2:"
ulimit -v "$4"
timeout -s "SIGTERM" "$3" ./build/pace --verify="./test/$1/solutions/$2.sol" < "./test/$1/instances/$2.gr" > /dev/null 2>/dev/null
exit_code=$?

if [ $exit_code -eq 139 ]; then
    echo "MLE"
elif [ $exit_code -eq 124 ]; then
    echo "TLE"
else
    echo "AC"
fi
