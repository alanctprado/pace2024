#!/bin/bash

if [ ! -d "./lp_solve_5.5/lpsolve55/bin/" ]
then
  cd ./lp_solve_5.5/lpsolve55/
  sh ccc
  cd -
fi
