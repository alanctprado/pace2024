#!/bin/bash

if [ $# -lt 4 ]; then
    echo "Usage: $0 <file> <tl> <ml> <args>"
    exit 1
fi

ARGS=$*

ulimit -v "$3"
timeout -s "SIGTERM" "$2" ./build/pace "$ARGS" < "$1" >/dev/null 2>/dev/null
exit_code=$?

if [ $exit_code -eq 139 ]; then
    echo "MLE"
elif [ $exit_code -eq 124 ]; then
    echo "TLE"
else
    echo "AC"
fi
