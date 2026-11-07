# dclang Primitive Reference

**Inventory:** 213 registered primitives (189 public, 24 private). All 213 have source-reviewed stack-effect signatures recorded in the canonical JSON registry. This is not a claim of exhaustive runtime verification.

## Verification terminology

- **Source-reviewed signature:** traced against the C implementation; may still have untested edge cases.
- **Runtime evidence:** individual observations or assertions are recorded in `../../tests/ai/primitives.json`; passing examples do not prove all inputs or error paths.
- **Not exhaustively verified:** a qualification of behavioral coverage, **not** a missing stack signature.

Stack notation is `( before -- after )`, with the top of the data stack on the right. `...` represents an untouched stack prefix. Private primitives begin with `_`. Source line numbers refer to the audited source snapshot and can change.

## Important semantics and pitfalls

- `pick` consumes a zero-based depth index: `0 pick` duplicates the top item; `1 pick` duplicates the next item.
- `2over` duplicates the lower pair of four stack values; `2dup` duplicates the top pair.
- `/` is floating-point division; `%` calls `fmod`, not integer modulo.
- `round`, `floor`, and `ceil` cast the mathematical result to `DCLANG_LONG` before pushing it.
- `svpush`/`svpop` transfer between the data stack and the separate save stack.
- `times ... again` and `for ... next` are compiled loop constructs, not free-standing REPL loops. See `LANGUAGE.md` for zero/negative count caveats.
- `if ... else ... endif` are compiled special forms.
- Library words may use different operand ordering than similarly named primitives. Verify against source and tests.

## Primitive inventory

The **Stack effect** column is generated directly from `data/primitives.json`, avoiding divergence between the Markdown and machine-readable registry. All entries below are source-reviewed; runtime coverage varies.

### Stack Ops

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `drop` | public | `( x -- )` | `OP_DROP` | 3422 |
| `dup` | public | `( x -- x x )` | `OP_DUP` | 3423 |
| `over` | public | `( a b -- a b a )` | `OP_OVER` | 3424 |
| `pick` | public | `( ... x_n ... x_0 n -- ... x_n ... x_0 x_n )` | `OP_PICK` | 3425 |
| `swap` | public | `( a b -- b a )` | `OP_SWAP` | 3426 |
| `2drop` | public | `( a b -- )` | `OP_DROP2` | 3427 |
| `2dup` | public | `( a b -- a b a b )` | `OP_DUP2` | 3428 |
| `2over` | public | `( a b c d -- a b c d a b )` | `OP_OVER2` | 3429 |
| `depth` | public | `( ... -- ... n )` | `OP_DEPTH` | 3430 |
| `clear` | public | `( ... -- )` | `OP_CLEAR` | 3431 |

### Save Stack Ops

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `svpush` | public | `( x -- ) [save: ... -- ... x]` | `OP_SVPUSH` | 3433 |
| `svpop` | public | `( -- x ) [save: ... x -- ...]` | `OP_SVPOP` | 3434 |
| `svdrop` | public | `( S: x -- )` | `OP_SVDROP` | 3435 |
| `svpick` | public | `( index -- value ; S: unchanged )` | `OP_SVPICK` | 3436 |
| `svdepth` | public | `( -- value )` | `OP_SVDEPTH` | 3437 |
| `svclear` | public | `( -- )` | `OP_SVCLEAR` | 3438 |

### Stack Output

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `.` | public | `( value -- )` | `OP_SHOW` | 3440 |
| `..` | public | `( value -- )` | `OP_SHOWNOSPACE` | 3441 |
| `.rj` | public | `( value width precision -- )` | `OP_SHOWRJ` | 3442 |
| `.pz` | public | `( value width precision -- )` | `OP_SHOWPZ` | 3443 |
| `.s` | public | `( -- )` | `OP_SHOWSTACK` | 3444 |

### Boolean

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `null` | public | `( -- value )` | `OP_NULL` | 3446 |
| `false` | public | `( -- value )` | `OP_FALSE` | 3447 |
| `true` | public | `( -- value )` | `OP_TRUE` | 3448 |
| `=` | public | `( a b -- result )` | `OP_EQ` | 3449 |
| `<>` | public | `( a b -- result )` | `OP_NOTEQ` | 3450 |
| `<` | public | `( a b -- result )` | `OP_LT` | 3451 |
| `>` | public | `( a b -- result )` | `OP_GT` | 3452 |
| `<=` | public | `( a b -- result )` | `OP_LTE` | 3453 |
| `>=` | public | `( a b -- result )` | `OP_GTE` | 3454 |
| `assert` | public | `( condition -- )` | `OP_ASSERT` | 3455 |

### Bit manipulation

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `and` | public | `( a b -- result )` | `OP_AND` | 3457 |
| `or` | public | `( a b -- result )` | `OP_OR` | 3458 |
| `xor` | public | `( a b -- result )` | `OP_XOR` | 3459 |
| `not` | public | `( bits -- complement )` | `OP_NOT` | 3460 |
| `<<` | public | `( value shift -- result )` | `OP_LSHIFT` | 3461 |
| `>>` | public | `( value shift -- result )` | `OP_RSHIFT` | 3462 |

### Arithmetic

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `+` | public | `( a b -- a+b )` | `OP_ADD` | 3464 |
| `-` | public | `( a b -- a-b )` | `OP_SUB` | 3465 |
| `*` | public | `( a b -- a*b )` | `OP_MUL` | 3466 |
| `/` | public | `( a b -- a/b )` | `OP_DIV` | 3467 |
| `%` | public | `( a b -- fmod(a,b) )` | `OP_MOD` | 3468 |
| `abs` | public | `( x -- abs(x) )` | `OP_ABS` | 3469 |
| `min` | public | `( a b -- min(a,b) )` | `OP_MIN` | 3470 |
| `max` | public | `( a b -- max(a,b) )` | `OP_MAX` | 3471 |

### Float -> Integer

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `round` | public | `( x -- round(x) )` | `OP_ROUND` | 3473 |
| `ceil` | public | `( x -- ceil(x) )` | `OP_CEIL` | 3474 |
| `floor` | public | `( x -- floor(x) )` | `OP_FLOOR` | 3475 |

### Higher Math

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `pow` | public | `( a b -- result )` | `OP_POWER` | 3477 |
| `sqrt` | public | `( x -- result )` | `OP_SQRT` | 3478 |
| `exp` | public | `( x -- result )` | `OP_EXP` | 3479 |
| `log` | public | `( x -- result )` | `OP_LOG` | 3480 |
| `log2` | public | `( x -- result )` | `OP_LOG2` | 3481 |
| `log10` | public | `( x -- result )` | `OP_LOG10` | 3482 |
| `e` | public | `( -- value )` | `OP_E` | 3483 |

### Trigonometry

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `pi` | public | `( -- value )` | `OP_PI` | 3485 |
| `sin` | public | `( x -- result )` | `OP_SINE` | 3486 |
| `cos` | public | `( x -- result )` | `OP_COS` | 3487 |
| `tan` | public | `( x -- result )` | `OP_TAN` | 3488 |
| `asin` | public | `( x -- result )` | `OP_ASINE` | 3489 |
| `acos` | public | `( x -- result )` | `OP_ACOS` | 3490 |
| `atan` | public | `( x -- result )` | `OP_ATAN` | 3491 |
| `atan2` | public | `( a b -- result )` | `OP_ATAN2` | 3492 |
| `sinh` | public | `( x -- result )` | `OP_SINH` | 3493 |
| `cosh` | public | `( x -- result )` | `OP_COSH` | 3494 |
| `tanh` | public | `( x -- result )` | `OP_TANH` | 3495 |

### Randomness

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `rand` | public | `( -- value )` | `OP_RAND` | 3497 |

### Variables & Arrays

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `var` | public | `( -- )` | `OP_VARIABLE` | 3499 |
| `!` | public | `( value slot -- )` | `OP_POKE` | 3500 |
| `@` | public | `( slot -- value )` | `OP_PEEK` | 3501 |
| `allot` | public | `( count -- )` | `OP_ALLOT` | 3502 |
| `create` | public | `( -- )` | `OP_CREATE` | 3503 |
| `,` | public | `( value -- )` | `OP_COMMA` | 3504 |
| `here` | public | `( -- slot )` | `OP_HERE` | 3505 |
| `const` | public | `( value -- )` | `OP_CONSTANT` | 3506 |
| `envget` | public | `( key-string -- value-string )` | `OP_ENVGET` | 3507 |
| `envset` | public | `( value-string key-string -- )` | `OP_ENVSET` | 3508 |

### Array Sorting

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `sortnums` | public | `( array-start-index count -- )` | `OP_SORTNUMS` | 3510 |
| `sortstrs` | public | `( array-start-index count -- )` | `OP_SORTSTRS` | 3511 |

### Hashes

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `h!` | public | `( value key-string -- )` | `OP_HASHSET` | 3513 |
| `h@` | public | `( key-string -- value )` | `OP_HASHGET` | 3514 |
| `hkeys` | public | `( key-index -- key-string )` | `OP_HASHKEYS` | 3515 |

### Trees

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `tmake` | public | `( -- tree-id )` | `OP_TREEMAKE` | 3517 |
| `t!` | public | `( tree-id key-string value -- value )` | `OP_TREESET` | 3518 |
| `t@` | public | `( tree-id key-string -- value )` | `OP_TREEGET` | 3519 |
| `twalk` | public | `( tree-index -- )` | `OP_TREEWALK` | 3520 |
| `tdel` | public | `( tree-id key-string -- )` | `OP_TREEDELETE` | 3521 |
| `tdestroy` | public | `( tree-id -- )` | `OP_TREEDESTROY` | 3522 |

### Lists

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `lmake` | public | `( -- list )` | `OP_LISTMAKE` | 3524 |
| `_lnext` | private | `( node-pointer -- next-node-pointer )` | `OP_LISTNEXT` | 3525 |
| `l!` | public | `( list index value -- )` | `OP_LISTSET` | 3526 |
| `l@` | public | `( list index -- value )` | `OP_LISTGET` | 3527 |
| `lpush` | public | `( list value -- )` | `OP_LISTPUSH` | 3528 |
| `lpop` | public | `( list -- value )` | `OP_LISTPOP` | 3529 |
| `lins` | public | `( list index value -- )` | `OP_LISTINSERT` | 3530 |
| `lrem` | public | `( list index -- )` | `OP_LISTREMOVE` | 3531 |
| `lsize` | public | `( list -- count )` | `OP_LISTSIZE` | 3532 |
| `ldel` | public | `( list -- )` | `OP_LISTDELETE` | 3533 |

### Branching

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `times` | public | `( count -- )` | `OP_TIMES` | 3535 |
| `again` | public | `( -- )` | `OP_AGAIN` | 3536 |
| `exittimes` | public | `( -- )` | `OP_EXITTIMES` | 3537 |
| `for` | public | `( limit start step -- )` | `OP_FOR` | 3538 |
| `next` | public | `( -- )` | `OP_NEXT` | 3539 |
| `exitfor` | public | `( -- )` | `OP_EXITFOR` | 3540 |
| `i` | public | `( -- current-loop-index )` | `OP_I` | 3541 |
| `j` | public | `( -- enclosing-loop-index )` | `OP_J` | 3542 |
| `k` | public | `( -- outer-loop-index )` | `OP_K` | 3543 |
| `if` | public | `( flag -- )` | `OP_IF` | 3544 |
| `else` | public | `( -- )` | `OP_ELSE` | 3545 |
| `endif` | public | `( -- )` | `OP_ENDIF` | 3546 |
| `return` | public | `( -- )` | `OP_RETURN` | 3547 |

### Time

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `clock` | public | `( -- timestamp-float )` | `OP_CLOCK` | 3549 |
| `epoch` | public | `( -- epoch-seconds )` | `OP_EPOCH` | 3550 |
| `epoch->dt` | public | `( epoch format-string -- formatted-string )` | `OP_EPOCH_TO_DT` | 3551 |
| `dt->epoch` | public | `( date-string format-string -- epoch-seconds )` | `OP_DT_TO_EPOCH` | 3552 |
| `sleep` | public | `( seconds -- )` | `OP_SLEEP` | 3553 |

### String Output

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `cr` | public | `( -- )` | `OP_CR` | 3555 |
| `print` | public | `( string -- )` | `OP_PRINT` | 3556 |

### Character Emitters

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `emit` | public | `( codepoint -- )` | `OP_EMIT` | 3558 |
| `uemit` | public | `( codepoint -- )` | `OP_UEMIT` | 3559 |
| `_stereo_out32` | private | `( left right -- )` | `OP_STEREO_OUT32` | 3560 |
| `_stereo_in32` | private | `( -- left right )` | `OP_STEREO_IN32` | 3561 |

### Character Types

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `isalnum` | public | `( x -- result )` | `OP_ISALNUM` | 3563 |
| `isalpha` | public | `( x -- result )` | `OP_ISALPHA` | 3564 |
| `iscntrl` | public | `( x -- result )` | `OP_ISCNTRL` | 3565 |
| `isdigit` | public | `( x -- result )` | `OP_ISDIGIT` | 3566 |
| `isgraph` | public | `( x -- result )` | `OP_ISGRAPH` | 3567 |
| `islower` | public | `( x -- result )` | `OP_ISLOWER` | 3568 |
| `isprint` | public | `( x -- result )` | `OP_ISPRINT` | 3569 |
| `ispunct` | public | `( x -- result )` | `OP_ISPUNCT` | 3570 |
| `isspace` | public | `( x -- result )` | `OP_ISSPACE` | 3571 |
| `isupper` | public | `( x -- result )` | `OP_ISUPPER` | 3572 |
| `isxdigit` | public | `( x -- result )` | `OP_ISXDIGIT` | 3573 |

### String Conversion

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `tohex` | public | `( x -- result )` | `OP_TOHEX` | 3575 |
| `tostr` | public | `( x -- result )` | `OP_TOSTR` | 3576 |
| `tonum` | public | `( x -- result )` | `OP_TONUM` | 3577 |
| `ord` | public | `( x -- result )` | `OP_ORD` | 3578 |
| `tolower` | public | `( x -- result )` | `OP_TOLOWER` | 3579 |
| `toupper` | public | `( x -- result )` | `OP_TOUPPER` | 3580 |

### String Ops

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `strlen` | public | `( x -- result )` | `OP_STRLEN` | 3582 |
| `str=` | public | `( a b -- result )` | `OP_STREQ` | 3583 |
| `str<` | public | `( a b -- result )` | `OP_STRLT` | 3584 |
| `str>` | public | `( a b -- result )` | `OP_STRGT` | 3585 |
| `strfind` | public | `( haystack needle -- pointer-or-null )` | `OP_STRFIND` | 3586 |
| `strspn` | public | `( string accepted-chars -- count )` | `OP_STRSPN` | 3587 |
| `strcspn` | public | `( string rejected-chars -- count )` | `OP_STRCSPN` | 3588 |
| `strtok` | public | `( string-or-null delimiters save-slot -- token-pointer )` | `OP_STRTOK` | 3589 |
| `mempcpy` | public | `( destination source size -- destination+size )` | `OP_MEMPCPY` | 3590 |
| `memset` | public | `( destination byte count -- destination )` | `OP_MEMSET` | 3591 |

### Memory

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `mkbuf` | public | `( size -- pointer )` | `OP_MKBUF` | 3593 |
| `free` | public | `( pointer -- )` | `OP_FREE` | 3594 |
| `mark` | public | `( -- )` | `OP_MARK` | 3595 |
| `rewind` | public | `( -- )` | `OP_REWIND` | 3596 |
| `memused` | public | `( -- fraction )` | `OP_MEMUSED` | 3597 |

### Files

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `import` | public | `( filename-string -- )` | `OP_IMPORT` | 3599 |
| `fopen` | public | `( filename mode -- file-pointer )` | `OP_FILEOPEN` | 3600 |
| `fmemopen` | public | `( buffer size mode -- file-pointer )` | `OP_FILEMEMOPEN` | 3601 |
| `fread` | public | `( destination byte-count file-pointer -- bytes-read )` | `OP_FILEREAD` | 3602 |
| `freadline` | public | `( file-pointer -- line-pointer byte-count )` | `OP_FILEREADLINE` | 3603 |
| `freadall` | public | `( file-pointer -- buffer-pointer byte-count )` | `OP_FILEREADALL` | 3604 |
| `fseek` | public | `( file-pointer offset whence -- )` | `OP_FILESEEK` | 3605 |
| `ftell` | public | `( file-pointer -- offset )` | `OP_FILETELL` | 3606 |
| `fwrite` | public | `( source byte-count file-pointer -- bytes-written )` | `OP_FILEWRITE` | 3607 |
| `fflush` | public | `( file-pointer -- )` | `OP_FILEFLUSH` | 3608 |
| `fclose` | public | `( file-pointer -- )` | `OP_FILECLOSE` | 3609 |
| `redirect` | public | `( file-pointer -- )` | `OP_REDIRECT` | 3610 |
| `seterr` | public | `( -- )` | `OP_SETERR` | 3611 |
| `setout` | public | `( -- )` | `OP_SETOUT` | 3612 |
| `flush` | public | `( -- )` | `OP_FLUSH` | 3613 |

### Files (no buffer)

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `open` | public | `( path flags -- fd )` | `OP_OPEN` | 3615 |
| `read` | public | `( fd buffer byte-count -- bytes-read )` | `OP_READ` | 3616 |
| `write` | public | `( fd buffer byte-count -- bytes-written )` | `OP_WRITE` | 3617 |
| `close` | public | `( fd -- status )` | `OP_CLOSE` | 3618 |

### Sockets

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `tcplisten` | public | `( port -- socket-fd )` | `OP_TCPLISTEN` | 3620 |
| `tcpaccept` | public | `( socket-fd -- client-fd )` | `OP_TCPACCEPT` | 3621 |
| `tcpconnect` | public | `( host port -- socket-fd )` | `OP_TCPCONNECT` | 3622 |
| `udpsocket` | public | `( -- socket-fd )` | `OP_UDPSOCKET` | 3623 |
| `udpbind` | public | `( socket-fd port -- status )` | `OP_UDPBIND` | 3624 |
| `udpconnect` | public | `( socket-fd host port -- status )` | `OP_UDPCONNECT` | 3625 |
| `udpsend` | public | `( socket-fd buffer byte-count -- bytes-sent )` | `OP_UDPSEND` | 3626 |
| `udprecv` | public | `( socket-fd timeout-ms max-bytes buffer -- bytes-received )` | `OP_UDPRECV` | 3627 |
| `udpclose` | public | `( socket-fd -- status )` | `OP_UDPCLOSE` | 3628 |

### Interrupt Blocking

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `block_sigint` | public | `( -- )` | `OP_BLOCKSIGINT` | 3630 |
| `unblock_sigint` | public | `( -- )` | `OP_UNBLOCKSIGINT` | 3631 |

### Operating System

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `fork` | public | `( -- process-id )` | `OP_FORK` | 3633 |
| `exit` | public | `( exit-code -- [process terminates] )` | `OP_EXIT` | 3634 |

### Info

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `primitives` | public | `( -- )` | `OP_SHOWPRIMITIVES` | 3636 |
| `words` | public | `( -- )` | `OP_SHOWWORDS` | 3637 |
| `constants` | public | `( -- )` | `OP_SHOWCONSTS` | 3638 |
| `variables` | public | `( -- )` | `OP_SHOWVARS` | 3639 |

### Dynamic Input

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `exec` | public | `( ... word_name -- ... ; effect depends on invoked word )` | `OP_EXEC` | 3641 |
| `input` | public | `( -- string )` | `OP_INPUT` | 3642 |

### Regex

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `_regcomp` | private | `( pattern regex-flags -- regex-pointer \| -1 )` | `_OP_REGCOMP` | 3644 |
| `_regexec` | private | `( regex-pointer input-string exec-flags -- matches-pointer \| -1 )` | `_OP_REGEXEC` | 3645 |
| `_regread` | private | `( matches-pointer match-index -- start end )` | `_OP_REGREAD` | 3646 |

### SQLite

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `_sqlite_open` | private | `( database-path -- database-pointer [on success] )` | `_OP_SQLITEOPEN` | 3648 |
| `_sqlite_prepare` | private | `( database-pointer sql-string -- statement-pointer [on success] )` | `_OP_SQLITEPREPARE` | 3649 |
| `_sqlite_step` | private | `( statement-pointer -- sqlite-result-code [on success] )` | `_OP_SQLITESTEP` | 3650 |
| `_sqlite_column` | private | `( statement-pointer column-index -- text-pointer [on success] )` | `_OP_SQLITECOLUMN` | 3651 |
| `_sqlite_finalize` | private | `( statement-pointer -- )` | `_OP_SQLITEFINALIZE` | 3652 |
| `_sqlite_exec` | private | `( database-pointer sql-string -- )` | `_OP_SQLITEEXEC` | 3653 |
| `_sqlite_close` | private | `( database-pointer -- )` | `_OP_SQLITECLOSE` | 3654 |

### MIDI

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `_pm_list` | private | `( -- )` | `_OP_PM_LIST` | 3656 |
| `_pm_open_out` | private | `( device-id -- )` | `_OP_PM_OPENOUT` | 3657 |
| `_pm_ws` | private | `( status data1 data2 -- )` | `_OP_PM_WS` | 3658 |
| `_pm_wsr` | private | `( data2 data1 status -- )` | `_OP_PM_WSR` | 3659 |
| `_pm_open_in` | private | `( device-id -- )` | `_OP_PM_OPENIN` | 3660 |
| `_pm_read` | private | `( -- data2 data1 status-or-result )` | `_OP_PM_READ` | 3661 |
| `_pm_close` | private | `( -- )` | `_OP_PM_CLOSE` | 3662 |
| `_pm_terminate` | private | `( -- )` | `_OP_PM_TERMINATE` | 3663 |

### OSC

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `_osc_put_str` | private | `( buffer offset string -- new-offset \| -1 )` | `_OP_OSC_PUT_STR` | 3665 |
| `_osc_put_i32` | private | `( buffer offset integer -- new-offset \| -1 )` | `_OP_OSC_PUT_I32` | 3666 |
| `_osc_put_f32` | private | `( buffer offset float -- new-offset \| -1 )` | `_OP_OSC_PUT_F32` | 3667 |

## Remaining verification work

The signature inventory is complete. Remaining work concerns **behavioral confidence**, especially argument types, bounds, failure paths, ownership, resource cleanup, side effects, compiled control flow, and external integrations (OSC, MIDI, SQLite, sockets). See `GOTCHAS.md` for known caveats. Test cases and historical provenance are retained in `../../tests/ai/primitives.json`.

Source-review findings are not automatically confirmed runtime defects.
