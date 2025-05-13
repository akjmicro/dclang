#!/bin/bash

if [[ $# -lt 1 ]]
then
  echo "You didn't give an example .dc file to run"
  exit 0
fi

example=$1
exampleword=$(echo ${example} | sed -E 's/\.dc//g')
cat "$example" \
  | egrep -v "^${exampleword}|inf times|again" \
  | sed -E 's/stereo_out/stereo_stack/g' \
  > _tmp_portaudio.dc

make clean

echo "#define DSP_FILE \"_tmp_portaudio.dc\""  >> portaudio_example.c
echo "#define DSP_WORD \"${exampleword}\""     >> portaudio_example.c
cat portaudio_stub.c                           >> portaudio_example.c

make
nice -19 ./portaudio_example
