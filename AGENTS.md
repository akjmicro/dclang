# dclang — AI agent instructions

Read `docs/ai/README.md` and `docs/ai/LANGUAGE.md` before editing `.dc` code. Consult `docs/ai/PRIMITIVES.md` for the complete 213-primitive inventory and `docs/ai/STDLIB.md` for standard-library words. Consult `docs/ai/GOTCHAS.md` before changing loops, strings, buffers, imports, or dynamic execution.

Machine-readable sources: `docs/ai/data/primitives.json` and `docs/ai/data/stdlib.json`. Reproducible historical evidence: `tests/ai/primitives.json`, `tests/ai/stdlib.json`. Run `python3 tests/ai/run.py --check` for schema and inventory integrity.

This language is **dclang**, not Forth. Do not infer stack effects or syntax from Forth conventions. Word definitions require `: name` (a space after the colon). Distinguish source-reviewed signatures from runtime-verified semantics. Do not treat exploratory mismatches as proven interpreter defects.
