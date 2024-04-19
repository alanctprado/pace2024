#!/bin/bash

for file in ./test/tiny-set/instances/* ; do
  echo $file
  ./build/pace < $file
  echo
done
