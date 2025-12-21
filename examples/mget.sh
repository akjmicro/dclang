#!/bin/bash
KEYDIR=/tmp/keys

for key in "$@"; do
  file="$KEYDIR/$key"
  if [ -f "$file" ]; then
    printf "%s: %s\n" "$key" "$(cat "$file")"
  else
    printf "%s: null\n" "$key"
  fi
done
