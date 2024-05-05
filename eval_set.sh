#!/bin/bash

if [ $# -lt 3 ]; then
    echo "Usage: $0 <set> <tl> <ml> <args>"
    exit 1
fi

out=/tmp/"$1".csv

if [ -f "$out" ]; then
    rm -f "$out"
fi

echo "# SET:$1 TL:$2 ML:$3" >> "$out"
echo "CASE, STATUS, TIME(s), MEMORY(KiB)" >> "$out"

# Run each instance from the given set
for file in ./test/"$1"/instances/*; do

    # Bash shenanigans for getting the file name without extensions
    file=${file##*/}
    file=${file%.*}

    echo "$file," "$(sh eval_case.sh "$1" "$file" "$2" "$3" | tail -n 1)," \
         "$(/usr/bin/time -f'%e\n%M' sh eval_case.sh "$1" "$file" "$2" "$3" 2>&1 | sed -n '3,4p' | tr '\n' ',')" >> "$out" &
done
