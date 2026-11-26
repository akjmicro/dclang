# dclang2c-dsp tooling

This directory contains the reusable C and C++ infrastructure for compiling a
bounded dclang DSP patch and hosting the generated code:

- `dclang2c-dsp.c`: host-side patch compiler.
- `local_render.c`: JACK host with optional config-driven OSC controls.
- `bela_render.cpp`: basic Bela render host.
- `bela_osc_render.cpp`: Bela host for the generic dclang OSC control channel.

`dclang2c-dsp` is deliberately not a general dclang-to-C compiler. Its entry
word must be a statically traceable, bounded stereo render operation obeying
the restrictions documented in `docs/ai/GOTCHAS.md`.

The `dclang_dsp.h`, `dclang_osc.h`, `dclang_fast_math.h`, and
`dclang_phiverb.h` headers remain at the repository root because their kernels
are shared by the interpreter and generated DSP code. They are not private to
this tool.

Instrument-specific `<instrument>_osc.conf` maps remain beside their `.dc`
patches under `examples/dsp_examples/`. The `local-dsp` Make target detects
that sibling file automatically.
