#!/bin/bash

set -e

if [[ $# -lt 1 ]]
then
  echo "Usage <some_example.dc> [alsa|pa|jack|portaudio]"
  exit 0
fi

EXAMPLE="$1"
if [[ "$EXAMPLE" != *.dc && -f "${EXAMPLE}.dc" ]]
then
  EXAMPLE="${EXAMPLE}.dc"
fi
EXAMPLE_DIR="$(cd "$(dirname "$EXAMPLE")" && pwd)"
EXAMPLE_FILE="$(basename "$EXAMPLE")"
cd "$EXAMPLE_DIR"
RATE="${SR:-44100}"
export SR="$RATE"
CHANNELS=2
FORMAT=s32
FORMAT_UPPER=S32_LE
LATENCY=128
ALSA_DEVICE="${ALSA_DEVICE:-plughw:0,0}"
ALSA_PERIOD_SIZE="${ALSA_PERIOD_SIZE:-128}"
ALSA_BUFFER_SIZE="${ALSA_BUFFER_SIZE:-256}"
PA_DEVICE="${PA_DEVICE:-0}"
PA_BUFFER_SIZE="${PA_BUFFER_SIZE:-128}"
PA_PREFILL_BUFFERS="${PA_PREFILL_BUFFERS:-4}"
JACK_CLIENT_NAME="${JACK_CLIENT_NAME:-dclang_stdin}"
JACK_PLAYBACK_1="${JACK_PLAYBACK_1:-system:playback_1}"
JACK_PLAYBACK_2="${JACK_PLAYBACK_2:-system:playback_2}"
PIPEWIRE_TARGET="dclang_mixer"
PLAYER="${2:-portaudio}"

case "$PLAYER" in
  alsa)
    echo "Using alsa"
    dclang sync.dc
    dclang -e "\"$EXAMPLE_FILE\" import ${EXAMPLE_FILE%.dc} " | tee out.raw | aplay \
      -D "$ALSA_DEVICE" \
      -r "$RATE" \
      -c "$CHANNELS" \
      -f "$FORMAT_UPPER" \
      --period-size="$ALSA_PERIOD_SIZE" \
      --buffer-size="$ALSA_BUFFER_SIZE"
    ;;
  pa)
    echo "Using portaudio (via ./pa_play)"
    dclang sync.dc
    dclang -e "\"$EXAMPLE_FILE\" import ${EXAMPLE_FILE%.dc} " | tee out.raw | ./pa_play \
      --rate "$RATE" \
      --channels "$CHANNELS" \
      --format "$FORMAT"le \
      --buffer "$PA_BUFFER_SIZE" \
      --prefill "$PA_PREFILL_BUFFERS" \
      --device "$PA_DEVICE"
    ;;
  jack)
    echo "Using JACK (via jack-stdin)"
    if ! command -v jack-stdin >/dev/null 2>&1 || ! command -v jack_samplerate >/dev/null 2>&1
    then
      echo "JACK playback requires jack-stdin and jack_samplerate."
      exit 1
    fi

    RATE="$(jack_samplerate)"
    export SR="$RATE"
    dclang sync.dc
    dclang -e "\"$EXAMPLE_FILE\" import ${EXAMPLE_FILE%.dc} " | tee out.raw | jack-stdin \
      --bitdepth 32 \
      --encoding signed-integer \
      --little-endian \
      --name "$JACK_CLIENT_NAME" \
      "$JACK_PLAYBACK_1" \
      "$JACK_PLAYBACK_2"
    ;;
  *)
    echo "Using PipeWire"
    if command -v pw-cat >/dev/null 2>&1 && command -v pw-link >/dev/null 2>&1
    then
      if ! pw-link -o | grep -q "^${PIPEWIRE_TARGET}"
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
      dclang -e "\"$EXAMPLE_FILE\" import ${EXAMPLE_FILE%.dc} " | tee out.raw | pw-cat -p -a \
        --rate "$RATE" \
        --channels "$CHANNELS" \
        --format "$FORMAT" \
        --latency "$LATENCY" \
        --target "$PIPEWIRE_TARGET" -
    else
      echo "You selected Pipewire, but no supported Pipewire audio player found."
      echo "Need PipeWire tools: pw-cat, pw-link, pw-cli"
      exit 1
    fi
    ;;
esac
