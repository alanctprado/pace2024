#!/bin/bash

if [ ! -d "./lp_solve_5.5/lpsolve55/bin/" ]
then
  pushd ./lp_solve_5.5/lpsolve55/
  sh ccc
  popd
fi
