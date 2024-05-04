#!/usr/bin/env bash

export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:$(pwd)/or-tools"

if [ ! -d or-tools ]; then
    wget https://github.com/google/or-tools/archive/refs/tags/v9.10.tar.gz -O ortools.tar.gz
    tar -xzf ortools.tar.gz
    mv or-tools-9.10 or-tools
    rm ortools.tar.gz
fi

pushd or-tools

cmake -S . -B ../ortools -DBUILD_DEPS=ON -DBUILD_SHARED_LIBS=ON
cmake --build ../ortools --config Release --target all -j4 -v
popd

# cd ../../..
