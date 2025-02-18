#!/bin/bash

for f in $(ls *.dc)
do
  echo "Running tests in file: $f"
  dclang $f
done
