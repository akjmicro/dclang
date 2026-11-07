# dclang Language Semantics

**Status:** AI reference draft v0.1  
**Evidence:** `README.md`, `token.c`, `libdclang.c` from the supplied source archive (2026-11-06).  
**Scope:** Core syntax and execution model, not a complete primitive reference.  
**Verification:** Source-reviewed unless explicitly marked as an executable test. No interpreter execution was performed for this draft.

## 1. Identity and programming model

dclang is a minimal, stack-based, reverse Polish notation (RPN) language. Its syntax and semantics are **not interchangeable with Forth**, despite shared vocabulary. Programs are sequences of whitespace-delimited tokens evaluated in order. A word consumes operands from and/or produces results on the data stack. User-defined words are compiled into interpreter instructions.

**AI rule:** Never infer the existence, argument order, or behavior of a word solely because a similarly named word exists in Forth, PostScript, or another concatenative language.

## 2. Tokens, comments, numbers, and strings

- Ordinary tokens are separated by whitespace (`token.c:get_token`).
- `#` starts a comment through the end of the line.
- Double quotes delimit string literals, which push a pointer/address value onto the stack. A string is not an implicit `(address length)` pair.
- Escapes implemented by `token.c:stringfunc`: `\\b`, `\\t`, `\\n`, `\\r`, `\\xHH`, `\\uHHHH`, and `\\UHHHHHHHH`. Other backslash-escaped characters are passed through literally by the current implementation.
- Numeric token parsing uses `strtod`; consult source before assuming integer-only semantics or strict rejection of trailing nonnumeric characters.
- Unknown tokens produce a syntax error; the current `compile_or_interpret` implementation clears the data stack on this path.

```dclang
# A comment
4 5 + .
"Hello there!" print cr
```

## 3. Data stack and output

The data stack is the central mechanism for passing arguments and results. Examples:

```dclang
4 5 + .              # prints 9, consuming the result
1 2 3 5 + 7 16 / .s  # shows the stack without consuming it
```

`.` pops and prints the top of stack. `.s` displays stack contents non-destructively. Do not treat printing as a universal non-destructive operation.

**Stack-effect convention for future reference:** `( before -- after )`, with the top of stack on the **right**. For example `dup ( x -- x x )`, `swap ( a b -- b a )`. These examples are conventional and must still be checked against implementation when generating the primitive registry.

## 4. User-defined words

A named word is declared with a colon, a whitespace-separated name, its body, and a semicolon:

```dclang
: square dup * ;
5 square .
```

The REPL recognizes `:` and `;` as structural tokens. Definitions compile their bodies; calling a defined word executes its compiled instructions. **A space after `:` is required**: write `: square`, not `:square`.

The compiler recognizes user-defined words, primitives, local variables, constants, variables, and numeric literals. Words must exist when referenced unless a supported self-reference is being compiled. The implementation has special handling for a definition referring to itself; do not generalize this to arbitrary forward declarations.

## 5. Conditionals

Inside a definition, use `if`, optionally `else`, and `endif`. `if` consumes a condition: zero is false and nonzero is true (`OP_JUMPZ`).

```dclang
: sign_label dup 0 > if "positive" else "not positive" endif ;
```

`if`, `else`, and `endif` are compilation-time special forms. **Use them inside compiled word definitions.** Do not infer that they are valid as free-standing interpreted control structures merely because their tokens are recognized.

## 6. Loops and counters

### `times ... again`

```dclang
: count7 7 times i . again ;
count7   # 0 1 2 3 4 5 6
```

The count is consumed at `times`; `i` exposes the innermost loop counter. `exittimes` exits a `times` loop. Loop forms `times`, `again`, `exittimes`, `for`, `next`, and `exitfor` are rejected outside definitions by `validate`.

**Current implementation caveat:** `OP_TIMES` enters the body unconditionally, and `OP_AGAIN` checks the count afterward. Consequently, a count of zero or less still executes the body once. This is a documented *current behavior/known pitfall*, not a recommended idiom. Guard nonpositive counts explicitly when required; do not assume zero iterations.

### `for ... next`

The README describes three arguments in this order: **limit, start, step** (its prose calls them “to/from/step”). The implementation pops `step`, then `start`, then `limit`.

```dclang
: count5 5 0 1 for i . next ;  # 0 1 2 3 4
```

`exitfor` exits a `for` loop. `i`, `j`, `k` access the current and outer loop counters, up to three levels as described in the README. The current implementation performs its termination test at `next`, so boundary behavior and step-zero cases require particular care. Do not assume a step of zero is safe.

## 7. Local variables

Locals are introduced within a definition by `{ name ... }`. The compiler emits instructions that pop incoming stack values into local slots; references to the names push their stored values. The implementation currently allocates eight local slots per call frame. See `libdclang.c:_repl`, `OP_SET_LOCAL`, and `OP_GET_LOCAL`.

**Open verification items:** Document a tested multi-argument example establishing exact name-to-stack order; verify local assignment spelling and nested-call behavior before recommending these patterns. The compiler recognizes local-name tokens and a suffix-`!` assignment pattern, but the exact supported spelling and edge cases deserve tests.

## 8. Variables, arrays, and constants

```dclang
var mynum
42 mynum !
mynum @ .

var myarray 4 allot
3 myarray !
5 myarray 1 + !
myarray 1 + @ .

create initialized 3 , 5 , 7 , 11 ,
```

Variables and arrays share a global cell-storage model. `var` declares a named slot; `allot` advances allocation for additional cells; `!` stores using `( value slot -- )`; `@` fetches using `( slot -- value )`. Arrays are zero-indexed and the README warns that bounds are not automatically protected. `create` with `,` provides an alternative sequential initialization pattern. `,` is an **operator**, not punctuation.

A constant is declared by placing its value on the stack and using `const` with a name:

```dclang
42 const ANSWER
ANSWER .
```

## 9. Different container operand orders are intentional

Do **not** impose one operand-order convention on every data structure.

- Global variables: `value slot !`, `slot @`.
- Global string hash: `value key h!`, `key h@`.
- Trees: `tree key value t!`, `tree key t@`.
- Lists: `list index l@`, `list value lpush`.

Lists and trees are intentionally **container-first** to support chaining:

```dclang
outer_list 0 l@ 1 l@
```

This retrieves index 1 from the list at index 0 of `outer_list`. The compositional order is a deliberate design decision described in the README. Consult primitive source for mutation and deletion signatures not shown here.

## 10. Imports and library dependencies

```dclang
"examples/some_primes.dc" import
```

Libraries are ordinary `.dc` files loaded using `import`. `DCLANG_LIBS` is the documented environment variable pointing to the library directory. **Do not assume a library word is available until its defining module has been imported**, unless a source-verified initialization path loads it automatically. Import paths and dependency conventions need a separate standard-library index.

## 11. Strings, addresses, and memory ownership

Strings are represented as address values on the stack. `token.c:stringfunc` allocates string storage from the interpreter memory pool and null-terminates it. The source comments explicitly warn that repeated string creation consumes memory unless reclaimed; reuse strings by binding them to `const` or `var` when appropriate. The language is not garbage-collected.

`mkbuf`, `free`, `mark`, `rewind`, and other allocation facilities must be documented with precise lifetime/ownership rules before an AI should generate complex memory-management code. Never assume Python-like automatic object lifetime.

## 12. Source-of-truth and AI validation rules

Before producing nontrivial dclang code:

1. Verify every primitive and library word exists in this version.
2. Trace the stack from left to right, with top of stack on the right.
3. Confirm operand order separately for arrays, hashes, trees, and lists.
4. Use control structures within compiled definitions.
5. Confirm library imports and dependencies.
6. Check allocation ownership and string lifetime.
7. Distinguish tested behavior from source-reviewed assumptions.
8. When possible, run the program with the actual interpreter and report its output.

## 13. Known issues and open questions

- `times` count <= 0 executes its body once in the current source.
- `for` termination is checked after body execution; test zero/negative/overshooting cases before depending on them.
- Exact local-variable assignment examples need executable confirmation.
- Error handling and stack underflow behavior are primitive-specific.
- Numeric coercion, representation, and string/address interactions need a dedicated type-system section.
- The primitive and standard-library inventories belong in separate reference documents; **this file does not claim to list all supported words**.

## 14. Source map

| Topic | Primary evidence |
| --- | --- |
| Intro, examples, composability | `README.md` |
| Tokenization, strings, comments | `token.c:get_token`, `token.c:stringfunc` |
| Definitions and locals declaration | `libdclang.c:_repl` |
| Word lookup and compilation | `libdclang.c:compile_or_interpret` |
| Loop execution | `libdclang.c:OP_TIMES`, `OP_AGAIN`, `OP_FOR`, `OP_NEXT` |
| Conditionals | `libdclang.c:OP_IF`, `OP_ELSE`, `OP_ENDIF`, `OP_JUMPZ` |
| Local runtime | `libdclang.c:OP_SET_LOCAL`, `OP_GET_LOCAL` |

*Next phase: confirm all illustrative examples by execution; generate the primitive inventory and library symbol index from source.*


## Boolean and bitwise semantics (runtime-observed, v0.6)

The interpreter uses `-1` as the canonical true value and `0` as false. Comparisons such as `2 3 <` and `2 2 =` yield `-1`. Branch conditions use nonzero truthiness. Bitwise `not` computes a complement (`0 not` yields `-1`, `1 not` yields `-2`), and `and`/`or` are bitwise, not Boolean-normalizing. Distinguish canonical Boolean values from arbitrary truthy integers. See `stdlib_boolean_and_edge_results.json`.
