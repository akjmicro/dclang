# dclang: Gotchas and compatibility notes

These are behavior observations from the cumulative audit. Consult the canonical references and tests before relying on edge cases.

- **`exec` is dispatch, not eval.** It executes a named word supplied as a string; both user-defined words and registered primitives have been exercised.
- **Truth values and bitwise operations.** Canonical true is `-1`, false `0`; `not` and `and` are bitwise, not normalized logical operators.
- **Compiled loops.** `times ... again` and `for ... next` belong inside definitions. A zero or negative `times` count has been observed to execute the body once.
- **Indexed stack mutation.** `sinc`, `sincn`, and `ssetn` consume their index before addressing the remaining stack. Index `0` means the top remaining item. Current `sincn` and `ssetn` code casts the supplied value through `DCLANG_PTR`; fractional behavior has not been established.
- **Fused variable operand order.** `+@` and `+inc!` take `base offset`. `+incn!` takes `increment base offset`; `+setn!` takes `value base offset`. Do not rearrange them to resemble `!` or arithmetic notation.
- **`factorial` zero case.** `0 factorial` was observed to return `0`, not the mathematical value `1`.
- **`gcd` name collision.** Both `math.dc` and `fractions.dc` define `gcd`; import order can matter.
- **`strslice` storage.** It returns a pointer into a shared buffer, overwritten by subsequent calls. The observed slice end is exclusive.
- **`strreplace` and `str+` buffers.** These are not ordinary immutable-string concatenation functions; callers must supply appropriate writable storage.
- **`fopen` failure display.** An unbuffered I/O test displayed a large unsigned-looking number for a failing file descriptor; signed representation remains under investigation.
- **Safety audit observations.** Potential unchecked indexing or NULL-pointer assumptions are documented as source-review concerns, not proven vulnerabilities.
- **`shuffle_cells`.** The exploratory test fixture was not conclusive; behavior remains unverified.
- **DSP sample rate is import-time state.** `dsp.dc` reads `SR` while imported and defaults to 44100. The playback script now passes the same `SR` to its sink. A generated `dclang2c` patch fixes the sample rate at compile time.
- **Phasor identity follows call order.** Oscillator wrappers allocate persistent phase implicitly by incrementing `_phasor_count`. `t+` resets the count for the next sample. Reordering, conditionally skipping, or moving oscillator calls can change which state slot a later oscillator uses.
- **Private DSP kernels are layout-sensitive.** `_phasor`, `_fm`, `_sineloop`, `_tri`, `_sawtooth`, `_delay`, `_port`, and `_reverb` accept normalized steps and internal memory addresses. `_reverb_cells` reports the storage needed by the generated Phiverb state. Prefer the public `dsp.dc` wrappers unless implementing or compiling the library itself.
- **Reverb state is a generated Faust object.** `make_reverb_struct` reserves that object in dclang's cell arena; do not assume the former eight-delay-line layout or manually inspect its fields. The public `reverb ( left right struct cutoff feedback -- wet-left wet-right )` effect is unchanged. Each named struct is an independent instance, and `_reverb` performs no callback-time allocation. Phiverb's eight comb lines use independent triangle modulation with the v6.5.5 FDN rates and time-domain depths; regenerate `dclang_phiverb.h` with `contrib/generate_phiverb.py` after changing the Faust source.
- **Fast math is approximate.** `fast_sin` accepts radians; `fast_sine` accepts oscillator frequency. Their measured maximum sine error is about `3.55e-6`. `fast_tanh` saturates at ±1 for inputs outside ±4; its measured maximum error on `[-4,4]` is about `6.71e-4`. Use the exact words when numerical identity matters.
- **`dclang2c` executes setup.** Compilation imports the source and runs its top-level setup in the host interpreter. Compile trusted inputs only. The selected entry must be bounded, leave exactly two samples, use balanced branches, and avoid loops, dynamic execution, and audio I/O.

See `PRIMITIVES.md`, `STDLIB.md`, and `tests/ai/*.json` for full context and exact recorded observations.
