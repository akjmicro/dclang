#!/bin/bash

set -u

mkdir -p "/tmp/keys"
DCLANG="${DCLANG:-dclang}"
DCLANG_TEST_TIMEOUT="${DCLANG_TEST_TIMEOUT:-10}"
status=0

if [[ "$("$DCLANG" -e '41 1 + .')" == "42 " ]]
then
  echo "OK: -e final token at EOF"
else
  echo "FAILED: -e final token at EOF"
fi

if [[ "$("$DCLANG" -e '1.0 f. 1.5 f. 1e20 f.')" == "1.0 1.5 1e+20 " ]]
then
  echo "OK: f. distinguishes floating output"
else
  echo "FAILED: f. distinguishes floating output"
  status=1
fi

cell_hex_width=$(( $(getconf LONG_BIT) / 4 ))
printf -v one_cell_hex '%0*x' "$cell_hex_width" 1
if [[ "$("$DCLANG" -e '1 .s')" == "data_stack: <1> 0x${one_cell_hex} "*$'\n'"save_stack: <0> " ]]
then
  echo "OK: .s displays type-neutral raw cells"
else
  echo "FAILED: .s displays type-neutral raw cells"
  status=1
fi

public_error_words=(
  '.' 'u.' 'f.' '..' 'u..' 'f..' '.rj' 'u.rj' 'f.rj' 'f.pz'
  'f=' 'f<>' 'f<' 'f>' 'f<=' 'f>='
  'f+' 'f-' 'f*' 'f/' 'f%' 'fabs' 'fmin' 'fmax'
  'fround' 'fceil' 'ffloor' 'i>f' 'f>i'
  'fpow' 'fsqrt' 'fexp' 'flog' 'flog2' 'flog10'
  'fsin' 'fcos' 'ftan' 'fasin' 'facos' 'fatan' 'fatan2'
  'fsinh' 'fcosh' 'ftanh'
)
public_error_names_ok=1
for word in "${public_error_words[@]}"
do
  if [[ "$("$DCLANG" -e "$word")" != "$word -- "* ]]
  then
    echo "FAILED: stale or missing underflow diagnostic for $word"
    public_error_names_ok=0
    status=1
  fi
done
if [[ $public_error_names_ok -eq 1 ]]
then
  echo "OK: typed underflow diagnostics use public names"
fi

if [[ "$("$DCLANG" -e '-1 .. 32 u.. 1.0 f.. 42 5 .rj 42 5 u.rj')" == '-1321.0   42    42 ' ]]
then
  echo "OK: typed compact and right-justified output"
else
  echo "FAILED: typed compact and right-justified output"
  status=1
fi

while IFS= read -r f
do
  output=$(timeout "$DCLANG_TEST_TIMEOUT" "$DCLANG" "$f" 2>&1)
  rc=$?
  if [[ $rc -eq 124 ]]; then
    echo "TIMEOUT: $f (${DCLANG_TEST_TIMEOUT}s)"
    status=1
  elif [[ $rc -ne 0 ]] || grep -Eq "ASSERT FAIL!|syntax error" <<<"$output"; then
    echo "FAILED: $f"
    status=1
  else
    echo "OK: $f"
  fi
done < <(find . -maxdepth 1 -name '*.dc' -printf '%f\n' | sort)

# Every DSP example with a bounded `render_frame` entry must emit exactly one
# stereo s32le frame (8 bytes) per iteration, with no textual diagnostics mixed
# into the stream.
dsp_frames=64
dsp_expected_bytes=$((dsp_frames * 8))
dsp_output=$(mktemp /tmp/dclang-dsp-examples.XXXXXX.raw)
for f in ../examples/dsp_examples/*.dc
do
  if grep -q '^: render_frame' "$f"
  then
    abs_f=$(realpath "$f")
    : > "$dsp_output"
    DCLANG_NO_MIDI=1 DCLANG_LIBS="${DCLANG_LIBS:-../lib}" \
      timeout "$DCLANG_TEST_TIMEOUT" "$DCLANG" -e \
      "\"$abs_f\" import : __render $dsp_frames times render_frame _stereo_out32 again ; __render" \
      > "$dsp_output" 2>&1
    rc=$?
    bytes=$(wc -c < "$dsp_output")
    if [[ $rc -eq 0 && $bytes -eq $dsp_expected_bytes ]]
    then
      echo "OK: dsp example $(basename "$f")"
    else
      echo "FAILED: dsp example $(basename "$f") (rc=$rc, bytes=$bytes)"
      status=1
    fi
  fi
done
rm -f "$dsp_output"

if [[ -x ../dclang2c-dsp ]]
then
  ./dclang2c_dsp_tests.sh || status=1
else
  echo "SKIP: dclang2c-dsp regression tests (compiler not built)"
fi

exit "$status"
