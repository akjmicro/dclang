#!/bin/bash

mkdir -p "/tmp/keys"

for f in $(ls *.dc)
do
  if dclang "$f" 2>&1 | grep -q "ASSERT FAIL!"; then
    echo "FAILED: $f"
  else
    echo "OK: $f"
  fi
done
