#!/bin/bash

if [ $# -ne 2 ]; then
    echo "Usage: $0 <set> <case>"
    exit 1
fi

mkdir -p /tmp/pace

# Run test case and compare output with solution
./build/pace < "./test/$1/instances/$2.gr" > "/tmp/pace/$2.out"
echo "diff for case $1/$2: "
diff -Z "./test/$1/solutions/$2.sol" "/tmp/pace/$2.out"
rm "/tmp/pace/$2.out"

rmdir /tmp/pace
