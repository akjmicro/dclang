#!/bin/bash

if [[ $# -lt 1 ]]
then
  echo "You didn't give an example .dc file to run"
  exit 0
fi

dclang $1 | aplay -f S32_LE -r 44100 -c 2
