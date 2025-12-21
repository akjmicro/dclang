#!/bin/bash

KEYDIR=/tmp/keys
mkdir -p "$KEYDIR"

while [ "$#" -gt 1 ]; do
  key="$1"
  val="$2"
  shift 2

  tmp="$KEYDIR/.${key}.tmp"
  printf "%s\n" "$val" > "$tmp" &&
  mv "$tmp" "$KEYDIR/$key"
done

