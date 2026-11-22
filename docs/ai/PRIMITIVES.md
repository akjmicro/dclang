# dclang Primitive Reference

**Inventory:** 236 registered primitives (198 public, 38 private). All 236 have source-reviewed stack-effect signatures recorded in the canonical JSON registry. This is not a claim of exhaustive runtime verification.

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
- `sinc`, `sincn`, and `ssetn` consume a zero-based stack-depth index after consuming their other operands. Index `0` targets the top remaining item. The current `sincn` and `ssetn` opcode bodies cast their value operand through `DCLANG_PTR`; fractional-value behavior is therefore not established by the integer assertion fixtures.
- `+@`, `+inc!`, `+incn!`, and `+setn!` address `base + offset`. Their operand order is shown in the inventory and differs from ordinary arithmetic phrasing.
- `times ... again` and `for ... next` are compiled loop constructs, not free-standing REPL loops. See `LANGUAGE.md` for zero/negative count caveats.
- `if ... else ... endif` are compiled special forms.
- Library words may use different operand ordering than similarly named primitives. Verify against source and tests.
- Private DSP primitives take normalized phase steps, memory slots, and layout details. Public programs should normally call the wrappers in `dsp.dc`.
- `_fast_sin` accepts radians and uses a ninth-order, phase-folded approximation; `_fast_tanh` uses a rational approximation with explicit saturation at ±4. Their public wrappers deliberately trade exact libm results for audio-rate speed.

## Primitive inventory

The **Stack effect** column is generated directly from `data/primitives.json`, avoiding divergence between the Markdown and machine-readable registry. All entries below are source-reviewed; runtime coverage varies.

### Stack Ops

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `drop` | public | `( x -- )` | `OP_DROP` | 3808 |
| `dup` | public | `( x -- x x )` | `OP_DUP` | 3809 |
| `over` | public | `( a b -- a b a )` | `OP_OVER` | 3810 |
| `pick` | public | `( ... x_n ... x_0 n -- ... x_n ... x_0 x_n )` | `OP_PICK` | 3811 |
| `swap` | public | `( a b -- b a )` | `OP_SWAP` | 3812 |
| `2drop` | public | `( a b -- )` | `OP_DROP2` | 3813 |
| `2dup` | public | `( a b -- a b a b )` | `OP_DUP2` | 3814 |
| `2over` | public | `( a b c d -- a b c d a b )` | `OP_OVER2` | 3815 |
| `depth` | public | `( ... -- ... n )` | `OP_DEPTH` | 3816 |
| `clear` | public | `( ... -- )` | `OP_CLEAR` | 3817 |
| `inc` | public | `( x -- x+1 )` | `OP_INC` | 3818 |
| `sinc` | public | `( ... x_n ... x_0 n -- ... x_n+1 ... x_0 )` | `OP_SINC` | 3819 |
| `sincn` | public | `( ... x_n ... x_0 increment n -- ... x_n+increment ... x_0 )` | `OP_SINCN` | 3820 |
| `ssetn` | public | `( ... x_n ... x_0 value n -- ... value ... x_0 )` | `OP_SSETN` | 3821 |

### Save Stack Ops

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `svpush` | public | `( x -- ) [save: ... -- ... x]` | `OP_SVPUSH` | 3823 |
| `svpop` | public | `( -- x ) [save: ... x -- ...]` | `OP_SVPOP` | 3824 |
| `svdrop` | public | `( S: x -- )` | `OP_SVDROP` | 3825 |
| `svpick` | public | `( index -- value ; S: unchanged )` | `OP_SVPICK` | 3826 |
| `svdepth` | public | `( -- value )` | `OP_SVDEPTH` | 3827 |
| `svclear` | public | `( -- )` | `OP_SVCLEAR` | 3828 |

### Stack Output

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `.` | public | `( value -- )` | `OP_SHOW` | 3830 |
| `..` | public | `( value -- )` | `OP_SHOWNOSPACE` | 3831 |
| `.rj` | public | `( value width precision -- )` | `OP_SHOWRJ` | 3832 |
| `.pz` | public | `( value width precision -- )` | `OP_SHOWPZ` | 3833 |
| `.s` | public | `( -- )` | `OP_SHOWSTACK` | 3834 |

### Boolean

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `null` | public | `( -- value )` | `OP_NULL` | 3836 |
| `false` | public | `( -- value )` | `OP_FALSE` | 3837 |
| `true` | public | `( -- value )` | `OP_TRUE` | 3838 |
| `=` | public | `( a b -- result )` | `OP_EQ` | 3839 |
| `<>` | public | `( a b -- result )` | `OP_NOTEQ` | 3840 |
| `<` | public | `( a b -- result )` | `OP_LT` | 3841 |
| `>` | public | `( a b -- result )` | `OP_GT` | 3842 |
| `<=` | public | `( a b -- result )` | `OP_LTE` | 3843 |
| `>=` | public | `( a b -- result )` | `OP_GTE` | 3844 |
| `assert` | public | `( condition -- )` | `OP_ASSERT` | 3845 |

### Bit manipulation

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `and` | public | `( a b -- result )` | `OP_AND` | 3847 |
| `or` | public | `( a b -- result )` | `OP_OR` | 3848 |
| `xor` | public | `( a b -- result )` | `OP_XOR` | 3849 |
| `not` | public | `( bits -- complement )` | `OP_NOT` | 3850 |
| `<<` | public | `( value shift -- result )` | `OP_LSHIFT` | 3851 |
| `>>` | public | `( value shift -- result )` | `OP_RSHIFT` | 3852 |

### Arithmetic

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `+` | public | `( a b -- a+b )` | `OP_ADD` | 3854 |
| `-` | public | `( a b -- a-b )` | `OP_SUB` | 3855 |
| `*` | public | `( a b -- a*b )` | `OP_MUL` | 3856 |
| `/` | public | `( a b -- a/b )` | `OP_DIV` | 3857 |
| `%` | public | `( a b -- fmod(a,b) )` | `OP_MOD` | 3858 |
| `abs` | public | `( x -- abs(x) )` | `OP_ABS` | 3859 |
| `min` | public | `( a b -- min(a,b) )` | `OP_MIN` | 3860 |
| `max` | public | `( a b -- max(a,b) )` | `OP_MAX` | 3861 |

### Float -> Integer

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `round` | public | `( x -- round(x) )` | `OP_ROUND` | 3863 |
| `ceil` | public | `( x -- ceil(x) )` | `OP_CEIL` | 3864 |
| `floor` | public | `( x -- floor(x) )` | `OP_FLOOR` | 3865 |

### Higher Math

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `pow` | public | `( a b -- result )` | `OP_POWER` | 3867 |
| `sqrt` | public | `( x -- result )` | `OP_SQRT` | 3868 |
| `exp` | public | `( x -- result )` | `OP_EXP` | 3869 |
| `log` | public | `( x -- result )` | `OP_LOG` | 3870 |
| `log2` | public | `( x -- result )` | `OP_LOG2` | 3871 |
| `log10` | public | `( x -- result )` | `OP_LOG10` | 3872 |
| `e` | public | `( -- value )` | `OP_E` | 3873 |

### Trigonometry

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `pi` | public | `( -- value )` | `OP_PI` | 3875 |
| `sin` | public | `( x -- result )` | `OP_SINE` | 3876 |
| `cos` | public | `( x -- result )` | `OP_COS` | 3877 |
| `tan` | public | `( x -- result )` | `OP_TAN` | 3878 |
| `asin` | public | `( x -- result )` | `OP_ASINE` | 3879 |
| `acos` | public | `( x -- result )` | `OP_ACOS` | 3880 |
| `atan` | public | `( x -- result )` | `OP_ATAN` | 3881 |
| `atan2` | public | `( a b -- result )` | `OP_ATAN2` | 3882 |
| `sinh` | public | `( x -- result )` | `OP_SINH` | 3883 |
| `cosh` | public | `( x -- result )` | `OP_COSH` | 3884 |
| `tanh` | public | `( x -- result )` | `OP_TANH` | 3885 |

### Randomness

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `rand` | public | `( -- value )` | `OP_RAND` | 3887 |

### Variables & Arrays

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `var` | public | `( -- )` | `OP_VARIABLE` | 3889 |
| `@` | public | `( slot -- value )` | `OP_PEEK` | 3890 |
| `+@` | public | `( base offset -- value )` | `OP_PEEKOFFSET` | 3891 |
| `!` | public | `( value slot -- )` | `OP_POKE` | 3892 |
| `inc!` | public | `( slot -- )` | `OP_POKE_INC` | 3893 |
| `+inc!` | public | `( base offset -- )` | `OP_POKEOFFSET_INC` | 3894 |
| `+incn!` | public | `( increment base offset -- )` | `OP_POKEOFFSET_INCN` | 3895 |
| `+setn!` | public | `( value base offset -- )` | `OP_POKEOFFSET_SETN` | 3896 |
| `allot` | public | `( count -- )` | `OP_ALLOT` | 3897 |
| `create` | public | `( -- )` | `OP_CREATE` | 3898 |
| `,` | public | `( value -- )` | `OP_COMMA` | 3899 |
| `here` | public | `( -- slot )` | `OP_HERE` | 3900 |
| `const` | public | `( value -- )` | `OP_CONSTANT` | 3901 |
| `envget` | public | `( key-string -- value-string )` | `OP_ENVGET` | 3902 |
| `envset` | public | `( value-string key-string -- )` | `OP_ENVSET` | 3903 |

### Array Sorting

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `sortnums` | public | `( array-start-index count -- )` | `OP_SORTNUMS` | 3905 |
| `sortstrs` | public | `( array-start-index count -- )` | `OP_SORTSTRS` | 3906 |

### Hashes

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `h!` | public | `( value key-string -- )` | `OP_HASHSET` | 3908 |
| `h@` | public | `( key-string -- value )` | `OP_HASHGET` | 3909 |
| `hkeys` | public | `( key-index -- key-string )` | `OP_HASHKEYS` | 3910 |

### Trees

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `tmake` | public | `( -- tree-id )` | `OP_TREEMAKE` | 3912 |
| `t!` | public | `( tree-id key-string value -- value )` | `OP_TREESET` | 3913 |
| `t@` | public | `( tree-id key-string -- value )` | `OP_TREEGET` | 3914 |
| `twalk` | public | `( tree-index -- )` | `OP_TREEWALK` | 3915 |
| `tdel` | public | `( tree-id key-string -- )` | `OP_TREEDELETE` | 3916 |
| `tdestroy` | public | `( tree-id -- )` | `OP_TREEDESTROY` | 3917 |

### Lists

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `lmake` | public | `( -- list )` | `OP_LISTMAKE` | 3919 |
| `_lnext` | private | `( node-pointer -- next-node-pointer )` | `OP_LISTNEXT` | 3920 |
| `l!` | public | `( list index value -- )` | `OP_LISTSET` | 3921 |
| `l@` | public | `( list index -- value )` | `OP_LISTGET` | 3922 |
| `lpush` | public | `( list value -- )` | `OP_LISTPUSH` | 3923 |
| `lpop` | public | `( list -- value )` | `OP_LISTPOP` | 3924 |
| `lins` | public | `( list index value -- )` | `OP_LISTINSERT` | 3925 |
| `lrem` | public | `( list index -- )` | `OP_LISTREMOVE` | 3926 |
| `lsize` | public | `( list -- count )` | `OP_LISTSIZE` | 3927 |
| `ldel` | public | `( list -- )` | `OP_LISTDELETE` | 3928 |

### Branching

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `times` | public | `( count -- )` | `OP_TIMES` | 3930 |
| `again` | public | `( -- )` | `OP_AGAIN` | 3931 |
| `exittimes` | public | `( -- )` | `OP_EXITTIMES` | 3932 |
| `for` | public | `( limit start step -- )` | `OP_FOR` | 3933 |
| `next` | public | `( -- )` | `OP_NEXT` | 3934 |
| `exitfor` | public | `( -- )` | `OP_EXITFOR` | 3935 |
| `i` | public | `( -- current-loop-index )` | `OP_I` | 3936 |
| `j` | public | `( -- enclosing-loop-index )` | `OP_J` | 3937 |
| `k` | public | `( -- outer-loop-index )` | `OP_K` | 3938 |
| `if` | public | `( flag -- )` | `OP_IF` | 3939 |
| `else` | public | `( -- )` | `OP_ELSE` | 3940 |
| `endif` | public | `( -- )` | `OP_ENDIF` | 3941 |
| `return` | public | `( -- )` | `OP_RETURN` | 3942 |

### Time

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `clock` | public | `( -- timestamp-float )` | `OP_CLOCK` | 3944 |
| `epoch` | public | `( -- epoch-seconds )` | `OP_EPOCH` | 3945 |
| `epoch->dt` | public | `( epoch format-string -- formatted-string )` | `OP_EPOCH_TO_DT` | 3946 |
| `dt->epoch` | public | `( date-string format-string -- epoch-seconds )` | `OP_DT_TO_EPOCH` | 3947 |
| `sleep` | public | `( seconds -- )` | `OP_SLEEP` | 3948 |

### String Output

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `cr` | public | `( -- )` | `OP_CR` | 3950 |
| `print` | public | `( string -- )` | `OP_PRINT` | 3951 |

### Character Emitters

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `emit` | public | `( codepoint -- )` | `OP_EMIT` | 3953 |
| `uemit` | public | `( codepoint -- )` | `OP_UEMIT` | 3954 |
| `_stereo_out32` | private | `( left right -- )` | `OP_STEREO_OUT32` | 3955 |
| `_stereo_in32` | private | `( -- left right )` | `OP_STEREO_IN32` | 3956 |

### Character Types

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `isalnum` | public | `( x -- result )` | `OP_ISALNUM` | 3958 |
| `isalpha` | public | `( x -- result )` | `OP_ISALPHA` | 3959 |
| `iscntrl` | public | `( x -- result )` | `OP_ISCNTRL` | 3960 |
| `isdigit` | public | `( x -- result )` | `OP_ISDIGIT` | 3961 |
| `isgraph` | public | `( x -- result )` | `OP_ISGRAPH` | 3962 |
| `islower` | public | `( x -- result )` | `OP_ISLOWER` | 3963 |
| `isprint` | public | `( x -- result )` | `OP_ISPRINT` | 3964 |
| `ispunct` | public | `( x -- result )` | `OP_ISPUNCT` | 3965 |
| `isspace` | public | `( x -- result )` | `OP_ISSPACE` | 3966 |
| `isupper` | public | `( x -- result )` | `OP_ISUPPER` | 3967 |
| `isxdigit` | public | `( x -- result )` | `OP_ISXDIGIT` | 3968 |

### String Conversion

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `tohex` | public | `( x -- result )` | `OP_TOHEX` | 3970 |
| `tostr` | public | `( x -- result )` | `OP_TOSTR` | 3971 |
| `tonum` | public | `( x -- result )` | `OP_TONUM` | 3972 |
| `ord` | public | `( x -- result )` | `OP_ORD` | 3973 |
| `tolower` | public | `( x -- result )` | `OP_TOLOWER` | 3974 |
| `toupper` | public | `( x -- result )` | `OP_TOUPPER` | 3975 |

### String Ops

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `strlen` | public | `( x -- result )` | `OP_STRLEN` | 3977 |
| `str=` | public | `( a b -- result )` | `OP_STREQ` | 3978 |
| `str<` | public | `( a b -- result )` | `OP_STRLT` | 3979 |
| `str>` | public | `( a b -- result )` | `OP_STRGT` | 3980 |
| `strfind` | public | `( haystack needle -- pointer-or-null )` | `OP_STRFIND` | 3981 |
| `strspn` | public | `( string accepted-chars -- count )` | `OP_STRSPN` | 3982 |
| `strcspn` | public | `( string rejected-chars -- count )` | `OP_STRCSPN` | 3983 |
| `strtok` | public | `( string-or-null delimiters save-slot -- token-pointer )` | `OP_STRTOK` | 3984 |
| `mempcpy` | public | `( destination source size -- destination+size )` | `OP_MEMPCPY` | 3985 |
| `memset` | public | `( destination byte count -- destination )` | `OP_MEMSET` | 3986 |

### Memory

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `mkbuf` | public | `( size -- pointer )` | `OP_MKBUF` | 3988 |
| `free` | public | `( pointer -- )` | `OP_FREE` | 3989 |
| `mark` | public | `( -- )` | `OP_MARK` | 3990 |
| `rewind` | public | `( -- )` | `OP_REWIND` | 3991 |
| `memused` | public | `( -- fraction )` | `OP_MEMUSED` | 3992 |

### Files

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `import` | public | `( filename-string -- )` | `OP_IMPORT` | 3994 |
| `fopen` | public | `( filename mode -- file-pointer )` | `OP_FILEOPEN` | 3995 |
| `fmemopen` | public | `( buffer size mode -- file-pointer )` | `OP_FILEMEMOPEN` | 3996 |
| `fread` | public | `( destination byte-count file-pointer -- bytes-read )` | `OP_FILEREAD` | 3997 |
| `freadline` | public | `( file-pointer -- line-pointer byte-count )` | `OP_FILEREADLINE` | 3998 |
| `freadall` | public | `( file-pointer -- buffer-pointer byte-count )` | `OP_FILEREADALL` | 3999 |
| `fseek` | public | `( file-pointer offset whence -- )` | `OP_FILESEEK` | 4000 |
| `ftell` | public | `( file-pointer -- offset )` | `OP_FILETELL` | 4001 |
| `fwrite` | public | `( source byte-count file-pointer -- bytes-written )` | `OP_FILEWRITE` | 4002 |
| `fflush` | public | `( file-pointer -- )` | `OP_FILEFLUSH` | 4003 |
| `fclose` | public | `( file-pointer -- )` | `OP_FILECLOSE` | 4004 |
| `redirect` | public | `( file-pointer -- )` | `OP_REDIRECT` | 4005 |
| `seterr` | public | `( -- )` | `OP_SETERR` | 4006 |
| `setout` | public | `( -- )` | `OP_SETOUT` | 4007 |
| `flush` | public | `( -- )` | `OP_FLUSH` | 4008 |

### Files (no buffer)

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `open` | public | `( path flags -- fd )` | `OP_OPEN` | 4010 |
| `read` | public | `( fd buffer byte-count -- bytes-read )` | `OP_READ` | 4011 |
| `write` | public | `( fd buffer byte-count -- bytes-written )` | `OP_WRITE` | 4012 |
| `close` | public | `( fd -- status )` | `OP_CLOSE` | 4013 |

### Sockets

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `tcplisten` | public | `( port -- socket-fd )` | `OP_TCPLISTEN` | 4015 |
| `tcpaccept` | public | `( socket-fd -- client-fd )` | `OP_TCPACCEPT` | 4016 |
| `tcpconnect` | public | `( host port -- socket-fd )` | `OP_TCPCONNECT` | 4017 |
| `udpsocket` | public | `( -- socket-fd )` | `OP_UDPSOCKET` | 4018 |
| `udpbind` | public | `( socket-fd port -- status )` | `OP_UDPBIND` | 4019 |
| `udpconnect` | public | `( socket-fd host port -- status )` | `OP_UDPCONNECT` | 4020 |
| `udpsend` | public | `( socket-fd buffer byte-count -- bytes-sent )` | `OP_UDPSEND` | 4021 |
| `udprecv` | public | `( socket-fd timeout-ms max-bytes buffer -- bytes-received )` | `OP_UDPRECV` | 4022 |
| `udpclose` | public | `( socket-fd -- status )` | `OP_UDPCLOSE` | 4023 |

### Interrupt Blocking

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `block_sigint` | public | `( -- )` | `OP_BLOCKSIGINT` | 4025 |
| `unblock_sigint` | public | `( -- )` | `OP_UNBLOCKSIGINT` | 4026 |

### Operating System

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `fork` | public | `( -- process-id )` | `OP_FORK` | 4028 |
| `exit` | public | `( exit-code -- [process terminates] )` | `OP_EXIT` | 4029 |

### Info

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `primitives` | public | `( -- )` | `OP_SHOWPRIMITIVES` | 4031 |
| `words` | public | `( -- )` | `OP_SHOWWORDS` | 4032 |
| `constants` | public | `( -- )` | `OP_SHOWCONSTS` | 4033 |
| `variables` | public | `( -- )` | `OP_SHOWVARS` | 4034 |

### Dynamic Input

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `exec` | public | `( ... word_name -- ... ; effect depends on invoked word )` | `OP_EXEC` | 4036 |
| `input` | public | `( -- string )` | `OP_INPUT` | 4037 |

### Regex

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `_regcomp` | private | `( pattern regex-flags -- regex-pointer \| -1 )` | `_OP_REGCOMP` | 4039 |
| `_regexec` | private | `( regex-pointer input-string exec-flags -- matches-pointer \| -1 )` | `_OP_REGEXEC` | 4040 |
| `_regread` | private | `( matches-pointer match-index -- start end )` | `_OP_REGREAD` | 4041 |

### SQLite

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `_sqlite_open` | private | `( database-path -- database-pointer [on success] )` | `_OP_SQLITEOPEN` | 4043 |
| `_sqlite_prepare` | private | `( database-pointer sql-string -- statement-pointer [on success] )` | `_OP_SQLITEPREPARE` | 4044 |
| `_sqlite_step` | private | `( statement-pointer -- sqlite-result-code [on success] )` | `_OP_SQLITESTEP` | 4045 |
| `_sqlite_column` | private | `( statement-pointer column-index -- text-pointer [on success] )` | `_OP_SQLITECOLUMN` | 4046 |
| `_sqlite_finalize` | private | `( statement-pointer -- )` | `_OP_SQLITEFINALIZE` | 4047 |
| `_sqlite_exec` | private | `( database-pointer sql-string -- )` | `_OP_SQLITEEXEC` | 4048 |
| `_sqlite_close` | private | `( database-pointer -- )` | `_OP_SQLITECLOSE` | 4049 |

### MIDI

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `_pm_list` | private | `( -- )` | `_OP_PM_LIST` | 4051 |
| `_pm_open_out` | private | `( device-id -- )` | `_OP_PM_OPENOUT` | 4052 |
| `_pm_ws` | private | `( status data1 data2 -- )` | `_OP_PM_WS` | 4053 |
| `_pm_wsr` | private | `( data2 data1 status -- )` | `_OP_PM_WSR` | 4054 |
| `_pm_open_in` | private | `( device-id -- )` | `_OP_PM_OPENIN` | 4055 |
| `_pm_read` | private | `( -- data2 data1 status-or-result )` | `_OP_PM_READ` | 4056 |
| `_pm_close` | private | `( -- )` | `_OP_PM_CLOSE` | 4057 |
| `_pm_terminate` | private | `( -- )` | `_OP_PM_TERMINATE` | 4058 |

### OSC

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `_osc_put_str` | private | `( buffer offset string -- new-offset \| -1 )` | `_OP_OSC_PUT_STR` | 4060 |
| `_osc_put_i32` | private | `( buffer offset integer -- new-offset \| -1 )` | `_OP_OSC_PUT_I32` | 4061 |
| `_osc_put_f32` | private | `( buffer offset float -- new-offset \| -1 )` | `_OP_OSC_PUT_F32` | 4062 |
| `_osc_get_str` | private | `( buffer length offset -- string-pointer new-offset \| 0 -1 )` | `_OP_OSC_GET_STR` | 4063 |
| `_osc_get_i32` | private | `( buffer length offset -- value new-offset \| 0 -1 )` | `_OP_OSC_GET_I32` | 4064 |
| `_osc_get_f32` | private | `( buffer length offset -- value new-offset \| 0 -1 )` | `_OP_OSC_GET_F32` | 4065 |

### DSP

| Word | Visibility | Stack effect | C opcode | Source line |
|---|---|---|---|---:|
| `_phasor` | private | `( step phase-base phase-count-slot -- old-phase )` | `_OP_DSP_PHASOR` | 4066 |
| `_delay` | private | `( delay-struct distance-samples -- sample )` | `_OP_DSP_DELAY` | 4067 |
| `_reverb` | private | `( left right struct cutoff feedback sample-rate -- wet-left wet-right )` | `_OP_DSP_REVERB` | 4068 |
| `_fm` | private | `( carrier-step modulator-step index phase-base phase-count-slot -- sample )` | `_OP_DSP_FM` | 4069 |
| `_sineloop` | private | `( state-slot step feedback phase-base phase-count-slot -- sample )` | `_OP_DSP_SINELOOP` | 4070 |
| `_tri` | private | `( step phase-base phase-count-slot -- sample )` | `_OP_DSP_TRI` | 4071 |
| `_sawtooth` | private | `( step phase-base phase-count-slot -- sample )` | `_OP_DSP_SAWTOOTH` | 4072 |
| `_port` | private | `( signal state-slot rise-alpha fall-alpha -- output )` | `_OP_DSP_PORT` | 4073 |
| `_fast_sin` | private | `( radians -- approximate-sine )` | `_OP_DSP_FAST_SIN` | 4074 |
| `_fast_tanh` | private | `( value -- approximate-tanh )` | `_OP_DSP_FAST_TANH` | 4075 |
| `_reverb_cells` | private | `( -- state-cells )` | `_OP_DSP_REVERB_CELLS` | 4076 |

## Remaining verification work

The signature inventory is complete. Remaining work concerns **behavioral confidence**, especially argument types, bounds, failure paths, ownership, resource cleanup, side effects, compiled control flow, and external integrations (OSC, MIDI, SQLite, sockets). See `GOTCHAS.md` for known caveats. Test cases and historical provenance are retained in `../../tests/ai/primitives.json`.

Source-review findings are not automatically confirmed runtime defects.
