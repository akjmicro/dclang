# Verification evidence

`primitives.json` and `stdlib.json` retain **all original case records** from the v1.8 bundle with their source filename and original index, plus source-audit reports. The original records are intentionally not rewritten or reclassified.

`python3 tests/ai/run.py --check` validates JSON and inventories. `--summary` prints counts and provenance. It does **not** claim to rerun historical tests; many records have different expression/output schemas and some require a particular binary or external system.

The `post_55a6a0d_source_audit_v2.0.json` record in `primitives.json` identifies the 20 primitives added through `3b333a8`; `fast_math_working_tree_v2.1.json` adds the two fast-math primitives, and `phiverb_working_tree_v2.2.json` records the generated reverb core and its private state-sizing primitive. The public stack and variable operations have executable assertions in `tests/stack_tests.dc` and `tests/variable_tests.dc`; those fixtures are evidence sources, not an implicit claim that this integrity checker reruns them.

To expand verification, normalize cases into an explicitly versioned runnable schema and record the binary hash and environment before reporting fresh passes.
