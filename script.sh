#!/bin/bash

TEST_CASES="./tests/exact-public/*"
PROGRAM="./build/pace"

N=35

function run_test() {
	timeout 30m $PROGRAM < $f > /dev/null;
	echo "$1 $?";
	echo "$1 $?" >> output2.txt;
}

for f in $TEST_CASES
do
	((i=i%N)); ((i++==0)) && wait
	run_test $f &
done
