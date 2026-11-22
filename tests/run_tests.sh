#!/bin/bash

mkdir -p "/tmp/keys"
DCLANG="${DCLANG:-dclang}"

if [[ "$("$DCLANG" -e '41 1 + .')" == "42 " ]]
then
  echo "OK: -e final token at EOF"
else
  echo "FAILED: -e final token at EOF"
fi

for f in $(ls *.dc)
do
  if "$DCLANG" "$f" 2>&1 | grep -q "ASSERT FAIL!"; then
    echo "FAILED: $f"
  else
    echo "OK: $f"
  fi
done
