# dclang: Gotchas and compatibility notes

These are behavior observations from the cumulative audit. Consult the canonical references and tests before relying on edge cases.

- **`exec` is dispatch, not eval.** It executes a named word supplied as a string; both user-defined words and registered primitives have been exercised.
- **Truth values and bitwise operations.** Canonical true is `-1`, false `0`; `not` and `and` are bitwise, not normalized logical operators.
- **Compiled loops.** `times ... again` and `for ... next` belong inside definitions. A zero or negative `times` count has been observed to execute the body once.
- **`factorial` zero case.** `0 factorial` was observed to return `0`, not the mathematical value `1`.
- **`gcd` name collision.** Both `math.dc` and `fractions.dc` define `gcd`; import order can matter.
- **`strslice` storage.** It returns a pointer into a shared buffer, overwritten by subsequent calls. The observed slice end is exclusive.
- **`strreplace` and `str+` buffers.** These are not ordinary immutable-string concatenation functions; callers must supply appropriate writable storage.
- **`fopen` failure display.** An unbuffered I/O test displayed a large unsigned-looking number for a failing file descriptor; signed representation remains under investigation.
- **Safety audit observations.** Potential unchecked indexing or NULL-pointer assumptions are documented as source-review concerns, not proven vulnerabilities.
- **`shuffle_cells`.** The exploratory test fixture was not conclusive; behavior remains unverified.

See `PRIMITIVES.md`, `STDLIB.md`, and `tests/ai/*.json` for full context and exact recorded observations.
