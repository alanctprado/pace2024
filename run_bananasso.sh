#!/bin/bash

for file in ./test/tiny-set/instances/* ; do
  echo $file
  ./a < $file
  echo
done
