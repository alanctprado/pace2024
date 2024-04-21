#!/bin/bash

if [ $# -ne 2 ]; then
    echo "Usage: $0 <set> <case>"
    exit 1
fi

mkdir -p /tmp/pace

# Run test case, the solution is expected to be verified inside the solver itself
echo "running for case $1/$2:"
./build/pace < "./test/$1/instances/$2.gr" > "/tmp/pace/$2.out"
rm "/tmp/pace/$2.out"

rmdir /tmp/pace
