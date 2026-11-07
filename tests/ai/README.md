# Verification evidence

`primitives.json` and `stdlib.json` retain **all original case records** from the v1.8 bundle with their source filename and original index, plus source-audit reports. The original records are intentionally not rewritten or reclassified.

`python3 tests/ai/run.py --check` validates JSON and inventories. `--summary` prints counts and provenance. It does **not** claim to rerun historical tests; many records have different expression/output schemas and some require a particular binary or external system.

To expand verification, normalize cases into an explicitly versioned runnable schema and record the binary hash and environment before reporting fresh passes.
