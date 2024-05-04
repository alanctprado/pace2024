#!/bin/bash

export GUROBI_HOME="$(pwd)/gurobi"
export PATH="${PATH}:${GUROBI_HOME}/bin"
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${GUROBI_HOME}/lib"
export GRB_LICENSE_FILE="${GUROBI_HOME}/gurobi.lic"

cd gurobi/src/build
make
cd ../../..
