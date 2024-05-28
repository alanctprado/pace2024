#!/usr/bin/bash

if [ $# -lt 4 ]; then
    echo "Usage: $0 <file> <tl> <ml> <args>"
    exit 1
fi

file=$1
tl=$2
ml=$3

shift 3
ARGS=$@

# echo "File: $file"
# echo "Tl: $tl"
# echo "ML: $ml"
# echo "Args: $ARGS"

(ulimit -v "$ml"; timeout -s "SIGTERM" "$tl" ./build/pace "$ARGS" < "$file" >/dev/null 2>/dev/null)
exit_code=$?

if [ $exit_code -eq 139 ]; then
    echo "MLE"
elif [ $exit_code -eq 124 ]; then
    echo "TLE"
elif [ $exit_code -eq 0 ]; then
    echo "AC"
else
    echo "Error: $exit_code"
fi
