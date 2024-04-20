#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: $0 <set>"
    exit 1
fi

# Run each instance from the given set
for file in ./test/$1/instances/*; do

    # Bash shenanigans for getting the file name without extensions
    file=${file##*/}
    file=${file%.*}

    sh run_case.sh "$1" "$file"
    echo
done
