#!/bin/bash

if [[ $# -lt 1 ]]
then
  echo "You didn't give an exmaple .dc file to run"
  exit 0
fi

example=$1
exampleword=$(echo ${example} | sed -E 's/\.dc//g')

make clean

echo "#define DSP_FILE \"${example}\""      >> portaudio_example.c
echo "#define DSP_WORD \"${exampleword}\""  >> portaudio_example.c
cat portaudio_stub.c                        >> portaudio_example.c

make
nice -19 ./portaudio_example
