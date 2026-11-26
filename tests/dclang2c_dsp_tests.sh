#!/bin/bash

set -u

ROOT=$(cd "$(dirname "$0")/.." && pwd)
DCLANG2C_DSP="${DCLANG2C_DSP:-$ROOT/dclang2c-dsp}"
CC="${CC:-cc}"
tmpdir=$(mktemp -d /tmp/dclang2c-dsp-tests.XXXXXX)
trap 'rm -rf "$tmpdir"' EXIT

if [[ ! -x "$DCLANG2C_DSP" ]]; then
  echo "SKIP: dclang2c-dsp is not built"
  exit 0
fi

status=0
for source in "$ROOT"/examples/dsp_examples/*.dc; do
  grep -q '^: render_frame' "$source" || continue
  [[ $(basename "$source") == sineloop_example.dc ]] && continue
  generated="$tmpdir/$(basename "${source%.dc}").c"
  if SR=44100 DCLANG2C_CELL_BITS=$(getconf LONG_BIT) DCLANG_LIBS="$ROOT/lib" \
       "$DCLANG2C_DSP" "$source" render_frame "$generated" &&
     "$CC" -O2 -I"$ROOT" -c "$generated" -o "$tmpdir/generated.o"
  then
    echo "OK: dclang2c-dsp $(basename "$source")"
  else
    echo "FAILED: dclang2c-dsp $(basename "$source")"
    status=1
  fi
done

generated="$tmpdir/panning-runtime.c"
if SR=44100 DCLANG2C_CELL_BITS=$(getconf LONG_BIT) DCLANG_LIBS="$ROOT/lib" \
     "$DCLANG2C_DSP" "$ROOT/examples/dsp_examples/panning_example.dc" render_frame "$generated" &&
   "$CC" -O2 -I"$ROOT" "$generated" "$ROOT/tests/dclang2c_dsp_harness.c" -lm \
     -o "$tmpdir/panning-runtime" &&
   "$tmpdir/panning-runtime"
then
  echo "OK: dclang2c-dsp float render runtime"
else
  echo "FAILED: dclang2c-dsp float render runtime"
  status=1
fi

# Cross-target output must carry the requested width guard even when the host
# cannot compile 32-bit objects locally.
generated="$tmpdir/reverb-32.c"
if SR=44100 DCLANG2C_CELL_BITS=32 DCLANG_LIBS="$ROOT/lib" \
     "$DCLANG2C_DSP" "$ROOT/examples/dsp_examples/reverb_example.dc" render_frame "$generated" &&
   grep -q 'UINTPTR_MAX != UINT32_MAX' "$generated" &&
   grep -q 'defined(DCLANG_MATH_NEON)' "$generated" &&
   grep -q 'int dclang2c_control_index' "$generated" &&
   grep -q 'void dclang2c_set_u32' "$generated" &&
   grep -q 'void dclang2c_set_float' "$generated" &&
   grep -q 'void dclang2c_render(dclang2c_sample \*output' "$generated"
then
  echo "OK: dclang2c-dsp 32-bit float target contract"
else
  echo "FAILED: dclang2c-dsp 32-bit float target contract"
  status=1
fi

exit "$status"
