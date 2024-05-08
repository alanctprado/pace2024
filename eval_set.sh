#!/bin/bash

if [ ! $# -eq 1 ]; then
    echo "Usage: $0 <file>"
    exit 1
fi

if [ ! "$(wc -l < $1)" -eq 5 ]; then
    echo "$1 does not have 5 lines!"
    exit 1
fi

path=$(sed -n '1 p' "$1")
tl=$(sed -n '2 p' "$1")
ml=$(sed -n '3 p' "$1")
jobs=$(sed -n '4 p' "$1")
args=$(sed -n '5 p' "$1")

out_name=${1##*/}
out_name=${out_name%.*}

out="$out_name".csv

if [ -f "$out" ]; then
    rm -f "$out"
fi

echo "# SET:$path TL:$tl ML:$ml ARGS:$args" >> "$out"
echo "CASE, STATUS, TIME(s), MEMORY(KiB)" >> "$out"

# Run each instance from the given set
for file in "$path"/*.gr; do
    cnt=$(ps | wc -l)

    while [[ "$cnt" -gt  "$jobs" ]]; do
        echo "Currently there are $cnt jobs running, waiting..."
        sleep "10s"
        cnt=$(ps | wc -l)
    done

    # Bash shenanigans for getting the file name without extensions
    name_file=${file##*/}
    name_file=${name_file%.*}

    echo "$name_file," "$(sh eval_case.sh "$file" "$tl" "$ml" "$args" | tail -n 1)," \
         "$(/usr/bin/time -f'%e\n%M' sh eval_case.sh "$file" "$tl" "$ml" "$args" 2>&1 | sed -n '2,3 p' | tr '\n' ',')" >> "$out" &
done
