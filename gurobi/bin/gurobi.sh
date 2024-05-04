#!/bin/sh

if test -z "${GUROBI_HOME}" ; then
  echo
  echo "Environment variable GUROBI_HOME is not set.  Consult"
  echo "https://www.gurobi.com/getting-started for information on how to set it."
  echo
fi

PATH=$GUROBI_HOME/bin:$PATH;export PATH
LD_LIBRARY_PATH=$GUROBI_HOME/lib:$LD_LIBRARY_PATH;export LD_LIBRARY_PATH
PYTHONHOME=$GUROBI_HOME;export PYTHONHOME

PYTHONSTARTUP=$PYTHONHOME/lib/gurobi.py;export PYTHONSTARTUP

$PYTHONHOME/bin/python3.11 "$@"
