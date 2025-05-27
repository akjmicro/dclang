#!/bin/bash

if [[ $# -lt 1 ]]
then
  echo "You didn't give an example .dc file to run"
  exit 0
fi

# This (`pw-cat` for `pipewire`) can be changed to `aplay` or similar as needed
# This also presumes a node called `dclang_mixer` has been created
dclang sync.dc
nice -19 dclang $1 | pw-cat -p -a \
            --rate 44100 \
            --channels 2 \
            --format s32 \
            --latency 128 \
            --target dclang_mixer -
