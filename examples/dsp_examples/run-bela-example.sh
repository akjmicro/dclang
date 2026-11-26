#!/bin/bash

set -euo pipefail

if [[ $# -lt 1 || $# -gt 2 ]]
then
  echo "Usage: $0 <example.dc> [entry-word]"
  echo "Environment overrides: BELA_HOST BELA_PROJECT BELA_RENDER SR"
  exit 1
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
SOURCE="$(realpath "$1")"
ENTRY_WORD="${2:-render_frame}"
SOURCE_NAME="$(basename "$SOURCE")"
PROJECT_DEFAULT="dclang-${SOURCE_NAME%.dc}"
PROJECT="${BELA_PROJECT:-$PROJECT_DEFAULT}"
RENDER="${BELA_RENDER:-tools/dclang2c-dsp/bela_render.cpp}"
CONTROLLER="${SOURCE%.dc}_controller.dc"

if [[ -f "$CONTROLLER" && -z "${BELA_RENDER:-}" ]]
then
  RENDER="tools/dclang2c-dsp/bela_osc_render.cpp"
fi

case "$SOURCE" in
  "$ROOT"/*) DSP_SOURCE="${SOURCE#"$ROOT"/}" ;;
  *)
    echo "Example must be inside the dclang repository: $SOURCE"
    exit 1
    ;;
esac

cd "$ROOT"

make -f Makefile.linux \
  DSP="$DSP_SOURCE" \
  ENTRY="$ENTRY_WORD" \
  BELA_RENDER="$RENDER" \
  BELA_PROJECT="$PROJECT" \
  bela-dsp

echo "Bela project built: $PROJECT"
echo "Start and stop it from an interactive Bela shell:"
echo "  ssh ${BELA_HOST:-root@192.168.7.2}"
echo "  cd ${BELA_HOME:-/root/Bela}/projects/$PROJECT"
echo "  ./$PROJECT"
if [[ -f "$CONTROLLER" ]]
then
  OSC_HOST="${BELA_HOST:-192.168.7.2}"
  OSC_HOST="${OSC_HOST#*@}"
  echo "In another shell, start the control bridge with:"
  echo "  BELA_HOST=$OSC_HOST dclang $CONTROLLER"
fi
