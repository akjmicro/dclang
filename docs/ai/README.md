# dclang AI reference

This is the **consolidated reference layout**. The canonical references are `LANGUAGE.md`, `PRIMITIVES.md`, `STDLIB.md`, and `GOTCHAS.md`. The registries in `data/` contain structured entries for 236 primitives and the standard-library module inventory.

The current references are reconciled through commit `3b333a8` plus the fast-math and Phiverb working tree. This includes the fused stack and variable operations, private OSC readers, native DSP kernels, fast sine and tanh approximations, the Faust-generated Phiverb core behind the existing public `reverb` interface, the updated `dsp.dc` wrappers, and the initial `dclang2c` ahead-of-time compiler.

`../../AGENTS.md` is the agent entry point. Test observations and audit provenance are retained in `../../tests/ai/`.

**Evidence caveat:** A recorded stack signature does not imply exhaustive runtime verification. The JSON records preserve the original verification labels and exploratory failures. Some historical tests may depend on an interpreter build or external services; the included runner supports listing and integrity checks, not automatic re-execution of every historical observation.

Do not add milestone-specific Markdown files to this directory. Update the canonical reference and structured evidence instead.
