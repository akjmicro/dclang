#!/bin/bash

set -e

if [[ $# -lt 1 ]]
then
  echo "You didn't give an example .dc file to run"
  exit 0
fi

EXAMPLE="$1"
RATE=48000
CHANNELS=2
FORMAT=s32
LATENCY=128
PIPEWIRE_TARGET="dclang_mixer"

if command -v pw-cat >/dev/null 2>&1 && command -v pw-link >/dev/null 2>&1
then
  echo "PipeWire detected."

  if ! pw-link -o | grep -q "^${PIPEWIRE_TARGET}$"
  then
    echo "Creating PipeWire null sink: ${PIPEWIRE_TARGET}"

    if command -v pw-cli >/dev/null 2>&1
    then
      pw-cli create-node adapter \
        '{ factory.name=support.null-audio-sink node.name="dclang_mixer" media.class=Audio/Sink object.linger=true audio.position=[ FL FR ] }' \
        >/dev/null
    else
      echo "PipeWire detected, but pw-cli is missing; cannot create ${PIPEWIRE_TARGET}."
      exit 1
    fi
  else
    echo "PipeWire target ${PIPEWIRE_TARGET} already exists."
  fi

  dclang sync.dc
  nice -19 dclang "$EXAMPLE" | pw-cat -p -a \
    --rate "$RATE" \
    --channels "$CHANNELS" \
    --format "$FORMAT" \
    --latency "$LATENCY" \
    --target "$PIPEWIRE_TARGET" -

elif command -v ./pa_play >/dev/null 2>&1
then
  echo "PipeWire not detected; using pa_play."

  dclang sync.dc
  nice -19 dclang "$EXAMPLE" | ./pa_play \
    --rate "$RATE" \
    --channels "$CHANNELS" \
    --format "$FORMAT"le

else
  echo "No supported audio player found."
  echo "Need either PipeWire tools: pw-cat, pw-link, pw-cli"
  echo "or pa_play."
  exit 1
fi
