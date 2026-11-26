# dclang Primitive Reference

**Inventory:** 267 registered primitives (225 public, 42 private). All entries have source-reviewed stack-effect signatures in the canonical JSON registry; runtime coverage is not exhaustive.

## Typed-cell semantics

- Every stack and variable slot is an untagged `uintptr_t` cell. Operators determine interpretation.
- Integer and floating-point operations are distinct; use `i>f` and `f>i` for numeric conversion.
- `.`, `u.`, and `f.` consume and print signed, unsigned, and floating interpretations. `.s` is non-destructive and prints raw fixed-width hexadecimal cells.
- `sortnums` sorts signed integer cells; `fsortnums` sorts packed floats with NaNs last. Neither converts mixed input.

## Verification terminology

- **Source-reviewed signature:** traced against the C implementation; edge cases may remain untested.
- **Runtime evidence:** historical observations are retained in `../../tests/ai/primitives.json`.

## Primitive inventory

### Stack Ops

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `drop` | public | `( x -- )` | `OP_DROP` | 4246 |
| `dup` | public | `( x -- x x )` | `OP_DUP` | 4247 |
| `over` | public | `( a b -- a b a )` | `OP_OVER` | 4248 |
| `pick` | public | `( ... x_n ... x_0 n -- ... x_n ... x_0 x_n )` | `OP_PICK` | 4249 |
| `swap` | public | `( a b -- b a )` | `OP_SWAP` | 4250 |
| `2drop` | public | `( a b -- )` | `OP_DROP2` | 4251 |
| `2dup` | public | `( a b -- a b a b )` | `OP_DUP2` | 4252 |
| `2over` | public | `( a b c d -- a b c d a b )` | `OP_OVER2` | 4253 |
| `depth` | public | `( ... -- ... n )` | `OP_DEPTH` | 4254 |
| `clear` | public | `( ... -- )` | `OP_CLEAR` | 4255 |
| `inc` | public | `( x -- x+1 )` | `OP_INC` | 4256 |
| `sinc` | public | `( ... x_n ... x_0 n -- ... x_n+1 ... x_0 )` | `OP_SINC` | 4257 |
| `sincn` | public | `( ... x_n ... x_0 increment n -- ... x_n+increment ... x_0 )` | `OP_SINCN` | 4258 |
| `ssetn` | public | `( ... x_n ... x_0 value n -- ... value ... x_0 )` | `OP_SSETN` | 4259 |

### Save Stack Ops

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `svpush` | public | `( x -- ) [save: ... -- ... x]` | `OP_SVPUSH` | 4261 |
| `svpop` | public | `( -- x ) [save: ... x -- ...]` | `OP_SVPOP` | 4262 |
| `svdrop` | public | `( S: x -- )` | `OP_SVDROP` | 4263 |
| `svpick` | public | `( index -- value ; S: unchanged )` | `OP_SVPICK` | 4264 |
| `svdepth` | public | `( -- value )` | `OP_SVDEPTH` | 4265 |
| `svclear` | public | `( -- )` | `OP_SVCLEAR` | 4266 |

### Stack Output

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `.` | public | `( value -- )` | `OP_SHOW` | 4268 |
| `u.` | public | `( value -- )` | `OP_SHOWUNSIGNED` | 4269 |
| `f.` | public | `( value -- )` | `OP_SHOWFLOAT` | 4270 |
| `..` | public | `( value -- )` | `OP_SHOWNOSPACE` | 4271 |
| `u..` | public | `( value -- )` | `OP_SHOWUNSIGNEDNOSPACE` | 4272 |
| `f..` | public | `( value -- )` | `OP_SHOWFLOATNOSPACE` | 4273 |
| `.rj` | public | `( value width -- )` | `OP_SHOWINTRJ` | 4274 |
| `u.rj` | public | `( value width -- )` | `OP_SHOWUNSIGNEDRJ` | 4275 |
| `f.rj` | public | `( value width precision -- )` | `OP_SHOWRJ` | 4276 |
| `f.pz` | public | `( value width precision -- )` | `OP_SHOWPZ` | 4277 |
| `.s` | public | `( -- )` | `OP_SHOWSTACK` | 4278 |

### Boolean

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `null` | public | `( -- value )` | `OP_NULL` | 4280 |
| `false` | public | `( -- value )` | `OP_FALSE` | 4281 |
| `true` | public | `( -- value )` | `OP_TRUE` | 4282 |
| `=` | public | `( a b -- result )` | `OP_EQ` | 4283 |
| `<>` | public | `( a b -- result )` | `OP_NOTEQ` | 4284 |
| `<` | public | `( a b -- result )` | `OP_LT` | 4285 |
| `>` | public | `( a b -- result )` | `OP_GT` | 4286 |
| `<=` | public | `( a b -- result )` | `OP_LTE` | 4287 |
| `>=` | public | `( a b -- result )` | `OP_GTE` | 4288 |
| `assert` | public | `( condition -- )` | `OP_ASSERT` | 4295 |

### Float Comparison

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `f=` | public | `( a b -- flag )` | `OP_FEQ` | 4289 |
| `f<>` | public | `( a b -- flag )` | `OP_FNOTEQ` | 4290 |
| `f<` | public | `( a b -- flag )` | `OP_FLT` | 4291 |
| `f>` | public | `( a b -- flag )` | `OP_FGT` | 4292 |
| `f<=` | public | `( a b -- flag )` | `OP_FLTE` | 4293 |
| `f>=` | public | `( a b -- flag )` | `OP_FGTE` | 4294 |

### Bit manipulation

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `and` | public | `( a b -- result )` | `OP_AND` | 4297 |
| `or` | public | `( a b -- result )` | `OP_OR` | 4298 |
| `xor` | public | `( a b -- result )` | `OP_XOR` | 4299 |
| `not` | public | `( bits -- complement )` | `OP_NOT` | 4300 |
| `<<` | public | `( value shift -- result )` | `OP_LSHIFT` | 4301 |
| `>>` | public | `( value shift -- result )` | `OP_RSHIFT` | 4302 |

### Arithmetic

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `+` | public | `( a b -- sum )` | `OP_ADD` | 4304 |
| `-` | public | `( a b -- difference )` | `OP_SUB` | 4305 |
| `*` | public | `( a b -- product )` | `OP_MUL` | 4306 |
| `/` | public | `( dividend divisor -- integer-quotient )` | `OP_DIV` | 4307 |
| `%` | public | `( dividend divisor -- integer-remainder )` | `OP_MOD` | 4308 |
| `/%` | public | `( dividend divisor -- quotient remainder )` | `OP_DIVMOD` | 4309 |
| `*/` | public | `( multiplicand multiplier divisor -- quotient )` | `OP_MULDIV` | 4310 |
| `*/%` | public | `( multiplicand multiplier divisor -- quotient remainder )` | `OP_MULDIVMOD` | 4311 |
| `abs` | public | `( integer -- magnitude )` | `OP_ABS` | 4312 |
| `min` | public | `( a b -- minimum )` | `OP_MIN` | 4313 |
| `max` | public | `( a b -- maximum )` | `OP_MAX` | 4314 |

### Float Arithmetic

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `f+` | public | `( a b -- a+b )` | `OP_FADD` | 4315 |
| `f-` | public | `( a b -- a-b )` | `OP_FSUB` | 4316 |
| `f*` | public | `( a b -- a*b )` | `OP_FMUL` | 4317 |
| `f/` | public | `( a b -- a/b )` | `OP_FDIV` | 4318 |
| `f%` | public | `( a b -- fmod(a,b) )` | `OP_FMOD` | 4319 |
| `fabs` | public | `( x -- \|x\| )` | `OP_FABS` | 4320 |
| `fmin` | public | `( a b -- min )` | `OP_FMIN` | 4321 |
| `fmax` | public | `( a b -- max )` | `OP_FMAX` | 4322 |

### Float -> Integer

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `fround` | public | `( x -- round(x) )` | `OP_ROUND` | 4324 |
| `fceil` | public | `( x -- ceil(x) )` | `OP_CEIL` | 4325 |
| `ffloor` | public | `( x -- floor(x) )` | `OP_FLOOR` | 4326 |

### Numeric Conversion

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `i>f` | public | `( integer -- float )` | `OP_ITOF` | 4327 |
| `f>i` | public | `( float -- integer )` | `OP_FTOI` | 4328 |

### Higher Math

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `fpow` | public | `( a b -- result )` | `OP_POWER` | 4330 |
| `fsqrt` | public | `( x -- result )` | `OP_SQRT` | 4331 |
| `fexp` | public | `( x -- result )` | `OP_EXP` | 4332 |
| `flog` | public | `( x -- result )` | `OP_LOG` | 4333 |
| `flog2` | public | `( x -- result )` | `OP_LOG2` | 4334 |
| `flog10` | public | `( x -- result )` | `OP_LOG10` | 4335 |
| `fe` | public | `( -- value )` | `OP_E` | 4336 |

### Trigonometry

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `fpi` | public | `( -- value )` | `OP_PI` | 4338 |
| `fsin` | public | `( x -- result )` | `OP_SINE` | 4339 |
| `fcos` | public | `( x -- result )` | `OP_COS` | 4340 |
| `ftan` | public | `( x -- result )` | `OP_TAN` | 4341 |
| `fasin` | public | `( x -- result )` | `OP_ASINE` | 4342 |
| `facos` | public | `( x -- result )` | `OP_ACOS` | 4343 |
| `fatan` | public | `( x -- result )` | `OP_ATAN` | 4344 |
| `fatan2` | public | `( a b -- result )` | `OP_ATAN2` | 4345 |
| `fsinh` | public | `( x -- result )` | `OP_SINH` | 4346 |
| `fcosh` | public | `( x -- result )` | `OP_COSH` | 4347 |
| `ftanh` | public | `( x -- result )` | `OP_TANH` | 4348 |

### Randomness

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `frand` | public | `( -- value )` | `OP_RAND` | 4350 |

### Variables & Arrays

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `var` | public | `( -- )` | `OP_VARIABLE` | 4352 |
| `@` | public | `( slot -- value )` | `OP_PEEK` | 4353 |
| `+@` | public | `( base offset -- value )` | `OP_PEEKOFFSET` | 4354 |
| `!` | public | `( value slot -- )` | `OP_POKE` | 4355 |
| `inc!` | public | `( slot -- )` | `OP_POKE_INC` | 4356 |
| `+inc!` | public | `( base offset -- )` | `OP_POKEOFFSET_INC` | 4357 |
| `+incn!` | public | `( increment base offset -- )` | `OP_POKEOFFSET_INCN` | 4358 |
| `+setn!` | public | `( value base offset -- )` | `OP_POKEOFFSET_SETN` | 4359 |
| `allot` | public | `( count -- )` | `OP_ALLOT` | 4360 |
| `create` | public | `( -- )` | `OP_CREATE` | 4361 |
| `,` | public | `( value -- )` | `OP_COMMA` | 4362 |
| `here` | public | `( -- slot )` | `OP_HERE` | 4363 |
| `const` | public | `( value -- )` | `OP_CONSTANT` | 4364 |
| `envget` | public | `( key-string -- value-string )` | `OP_ENVGET` | 4365 |
| `envset` | public | `( value-string key-string -- )` | `OP_ENVSET` | 4366 |

### Array Sorting

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `sortnums` | public | `( array-start-index count -- )` | `OP_SORTNUMS` | 4368 |
| `fsortnums` | public | `( array-start-index count -- )` | `OP_SORTFLOATS` | 4369 |
| `sortstrs` | public | `( array-start-index count -- )` | `OP_SORTSTRS` | 4370 |

### Hashes

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `h!` | public | `( value key-string -- )` | `OP_HASHSET` | 4372 |
| `h@` | public | `( key-string -- value )` | `OP_HASHGET` | 4373 |
| `hkeys` | public | `( key-index -- key-string )` | `OP_HASHKEYS` | 4374 |

### Trees

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `tmake` | public | `( -- tree-id )` | `OP_TREEMAKE` | 4376 |
| `t!` | public | `( tree-id key-string value -- value )` | `OP_TREESET` | 4377 |
| `t@` | public | `( tree-id key-string -- value )` | `OP_TREEGET` | 4378 |
| `twalk` | public | `( tree-index -- )` | `OP_TREEWALK` | 4379 |
| `tdel` | public | `( tree-id key-string -- )` | `OP_TREEDELETE` | 4380 |
| `tdestroy` | public | `( tree-id -- )` | `OP_TREEDESTROY` | 4381 |

### Lists

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `lmake` | public | `( -- list )` | `OP_LISTMAKE` | 4383 |
| `_lnext` | private | `( node-pointer -- next-node-pointer )` | `OP_LISTNEXT` | 4384 |
| `l!` | public | `( list index value -- )` | `OP_LISTSET` | 4385 |
| `l@` | public | `( list index -- value )` | `OP_LISTGET` | 4386 |
| `lpush` | public | `( list value -- )` | `OP_LISTPUSH` | 4387 |
| `lpop` | public | `( list -- value )` | `OP_LISTPOP` | 4388 |
| `lins` | public | `( list index value -- )` | `OP_LISTINSERT` | 4389 |
| `lrem` | public | `( list index -- )` | `OP_LISTREMOVE` | 4390 |
| `lsize` | public | `( list -- count )` | `OP_LISTSIZE` | 4391 |
| `ldel` | public | `( list -- )` | `OP_LISTDELETE` | 4392 |

### Branching

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `times` | public | `( count -- )` | `OP_TIMES` | 4394 |
| `again` | public | `( -- )` | `OP_AGAIN` | 4395 |
| `exittimes` | public | `( -- )` | `OP_EXITTIMES` | 4396 |
| `for` | public | `( limit start step -- )` | `OP_FOR` | 4397 |
| `next` | public | `( -- )` | `OP_NEXT` | 4398 |
| `exitfor` | public | `( -- )` | `OP_EXITFOR` | 4399 |
| `i` | public | `( -- current-loop-index )` | `OP_I` | 4400 |
| `j` | public | `( -- enclosing-loop-index )` | `OP_J` | 4401 |
| `k` | public | `( -- outer-loop-index )` | `OP_K` | 4402 |
| `if` | public | `( flag -- )` | `OP_IF` | 4403 |
| `else` | public | `( -- )` | `OP_ELSE` | 4404 |
| `endif` | public | `( -- )` | `OP_ENDIF` | 4405 |
| `return` | public | `( -- )` | `OP_RETURN` | 4406 |

### Time

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `clock` | public | `( -- epoch-seconds nanoseconds )` | `OP_CLOCK` | 4539 |
| `monoclock` | public | `( -- elapsed-seconds-float )` | `OP_MONOCLOCK` | 4540 |
| `_clock+` | private | `( epoch-seconds nanoseconds duration-float -- new-seconds new-nanoseconds )` | `_OP_CLOCK_ADD` | 4541 |
| `_clock%` | private | `( epoch-seconds nanoseconds period-float -- phase-float )` | `_OP_CLOCK_MOD` | 4542 |
| `_sleep_until` | private | `( epoch-seconds nanoseconds -- )` | `_OP_SLEEP_UNTIL` | 4543 |
| `epoch` | public | `( -- epoch-seconds )` | `OP_EPOCH` | 4409 |
| `epoch->dt` | public | `( epoch format-string -- formatted-string )` | `OP_EPOCH_TO_DT` | 4410 |
| `dt->epoch` | public | `( date-string format-string -- epoch-seconds )` | `OP_DT_TO_EPOCH` | 4411 |
| `sleep` | public | `( seconds -- )` | `OP_SLEEP` | 4412 |

### String Output

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `cr` | public | `( -- )` | `OP_CR` | 4414 |
| `print` | public | `( string -- )` | `OP_PRINT` | 4415 |

### Character Emitters

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `emit` | public | `( codepoint -- )` | `OP_EMIT` | 4417 |
| `uemit` | public | `( codepoint -- )` | `OP_UEMIT` | 4418 |
| `_stereo_out32` | private | `( left right -- )` | `OP_STEREO_OUT32` | 4419 |
| `_stereo_in32` | private | `( -- left right )` | `OP_STEREO_IN32` | 4420 |

### Character Types

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `isalnum` | public | `( x -- result )` | `OP_ISALNUM` | 4422 |
| `isalpha` | public | `( x -- result )` | `OP_ISALPHA` | 4423 |
| `iscntrl` | public | `( x -- result )` | `OP_ISCNTRL` | 4424 |
| `isdigit` | public | `( x -- result )` | `OP_ISDIGIT` | 4425 |
| `isgraph` | public | `( x -- result )` | `OP_ISGRAPH` | 4426 |
| `islower` | public | `( x -- result )` | `OP_ISLOWER` | 4427 |
| `isprint` | public | `( x -- result )` | `OP_ISPRINT` | 4428 |
| `ispunct` | public | `( x -- result )` | `OP_ISPUNCT` | 4429 |
| `isspace` | public | `( x -- result )` | `OP_ISSPACE` | 4430 |
| `isupper` | public | `( x -- result )` | `OP_ISUPPER` | 4431 |
| `isxdigit` | public | `( x -- result )` | `OP_ISXDIGIT` | 4432 |

### String Conversion

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `tohex` | public | `( x -- result )` | `OP_TOHEX` | 4434 |
| `tostr` | public | `( x -- result )` | `OP_TOSTR` | 4435 |
| `tonum` | public | `( x -- result )` | `OP_TONUM` | 4436 |
| `ord` | public | `( x -- result )` | `OP_ORD` | 4437 |
| `tolower` | public | `( x -- result )` | `OP_TOLOWER` | 4438 |
| `toupper` | public | `( x -- result )` | `OP_TOUPPER` | 4439 |

### String Ops

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `strlen` | public | `( x -- result )` | `OP_STRLEN` | 4441 |
| `str=` | public | `( a b -- result )` | `OP_STREQ` | 4442 |
| `str<` | public | `( a b -- result )` | `OP_STRLT` | 4443 |
| `str>` | public | `( a b -- result )` | `OP_STRGT` | 4444 |
| `strfind` | public | `( haystack needle -- pointer-or-null )` | `OP_STRFIND` | 4445 |
| `strspn` | public | `( string accepted-chars -- count )` | `OP_STRSPN` | 4446 |
| `strcspn` | public | `( string rejected-chars -- count )` | `OP_STRCSPN` | 4447 |
| `strtok` | public | `( string-or-null delimiters save-slot -- token-pointer )` | `OP_STRTOK` | 4448 |
| `mempcpy` | public | `( destination source size -- destination+size )` | `OP_MEMPCPY` | 4449 |
| `memset` | public | `( destination byte count -- destination )` | `OP_MEMSET` | 4450 |

### Memory

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `mkbuf` | public | `( size -- pointer )` | `OP_MKBUF` | 4452 |
| `free` | public | `( pointer -- )` | `OP_FREE` | 4453 |
| `mark` | public | `( -- )` | `OP_MARK` | 4454 |
| `rewind` | public | `( -- )` | `OP_REWIND` | 4455 |
| `memused` | public | `( -- fraction )` | `OP_MEMUSED` | 4456 |

### Files

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `import` | public | `( filename-string -- )` | `OP_IMPORT` | 4458 |
| `fopen` | public | `( filename mode -- file-pointer )` | `OP_FILEOPEN` | 4459 |
| `fmemopen` | public | `( buffer size mode -- file-pointer )` | `OP_FILEMEMOPEN` | 4460 |
| `fread` | public | `( destination byte-count file-pointer -- bytes-read )` | `OP_FILEREAD` | 4461 |
| `freadline` | public | `( file-pointer -- line-pointer byte-count )` | `OP_FILEREADLINE` | 4462 |
| `freadall` | public | `( file-pointer -- buffer-pointer byte-count )` | `OP_FILEREADALL` | 4463 |
| `fseek` | public | `( file-pointer offset whence -- )` | `OP_FILESEEK` | 4464 |
| `ftell` | public | `( file-pointer -- offset )` | `OP_FILETELL` | 4465 |
| `fwrite` | public | `( source byte-count file-pointer -- bytes-written )` | `OP_FILEWRITE` | 4466 |
| `fflush` | public | `( file-pointer -- )` | `OP_FILEFLUSH` | 4467 |
| `fclose` | public | `( file-pointer -- )` | `OP_FILECLOSE` | 4468 |
| `redirect` | public | `( file-pointer -- )` | `OP_REDIRECT` | 4469 |
| `seterr` | public | `( -- )` | `OP_SETERR` | 4470 |
| `setout` | public | `( -- )` | `OP_SETOUT` | 4471 |
| `flush` | public | `( -- )` | `OP_FLUSH` | 4472 |

### Files (no buffer)

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `open` | public | `( path flags -- fd )` | `OP_OPEN` | 4474 |
| `read` | public | `( fd buffer byte-count -- bytes-read )` | `OP_READ` | 4475 |
| `write` | public | `( fd buffer byte-count -- bytes-written )` | `OP_WRITE` | 4476 |
| `close` | public | `( fd -- status )` | `OP_CLOSE` | 4477 |

### Sockets

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `tcplisten` | public | `( port -- socket-fd )` | `OP_TCPLISTEN` | 4479 |
| `tcpaccept` | public | `( socket-fd -- client-fd )` | `OP_TCPACCEPT` | 4480 |
| `tcpconnect` | public | `( host port -- socket-fd )` | `OP_TCPCONNECT` | 4481 |
| `udpsocket` | public | `( -- socket-fd )` | `OP_UDPSOCKET` | 4482 |
| `udpbind` | public | `( socket-fd port -- status )` | `OP_UDPBIND` | 4483 |
| `udpconnect` | public | `( socket-fd host port -- status )` | `OP_UDPCONNECT` | 4484 |
| `udpsend` | public | `( socket-fd buffer byte-count -- bytes-sent )` | `OP_UDPSEND` | 4485 |
| `udprecv` | public | `( socket-fd timeout-ms max-bytes buffer -- bytes-received )` | `OP_UDPRECV` | 4486 |
| `udpclose` | public | `( socket-fd -- status )` | `OP_UDPCLOSE` | 4487 |

### Interrupt Blocking

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `block_sigint` | public | `( -- )` | `OP_BLOCKSIGINT` | 4489 |
| `unblock_sigint` | public | `( -- )` | `OP_UNBLOCKSIGINT` | 4490 |

### Operating System

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `fork` | public | `( -- process-id )` | `OP_FORK` | 4492 |
| `exit` | public | `( exit-code -- [process terminates] )` | `OP_EXIT` | 4493 |

### Info

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `primitives` | public | `( -- )` | `OP_SHOWPRIMITIVES` | 4495 |
| `words` | public | `( -- )` | `OP_SHOWWORDS` | 4496 |
| `constants` | public | `( -- )` | `OP_SHOWCONSTS` | 4497 |
| `variables` | public | `( -- )` | `OP_SHOWVARS` | 4498 |

### Dynamic Input

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `exec` | public | `( ... word_name -- ... ; effect depends on invoked word )` | `OP_EXEC` | 4500 |
| `input` | public | `( -- string )` | `OP_INPUT` | 4501 |

### Regex

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `_regcomp` | private | `( pattern regex-flags -- regex-pointer \| -1 )` | `_OP_REGCOMP` | 4503 |
| `_regexec` | private | `( regex-pointer input-string exec-flags -- matches-pointer \| -1 )` | `_OP_REGEXEC` | 4504 |
| `_regread` | private | `( matches-pointer match-index -- start end )` | `_OP_REGREAD` | 4505 |

### SQLite

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `_sqlite_open` | private | `( database-path -- database-pointer [on success] )` | `_OP_SQLITEOPEN` | 4507 |
| `_sqlite_prepare` | private | `( database-pointer sql-string -- statement-pointer [on success] )` | `_OP_SQLITEPREPARE` | 4508 |
| `_sqlite_step` | private | `( statement-pointer -- sqlite-result-code [on success] )` | `_OP_SQLITESTEP` | 4509 |
| `_sqlite_column` | private | `( statement-pointer column-index -- text-pointer [on success] )` | `_OP_SQLITECOLUMN` | 4510 |
| `_sqlite_finalize` | private | `( statement-pointer -- )` | `_OP_SQLITEFINALIZE` | 4511 |
| `_sqlite_exec` | private | `( database-pointer sql-string -- )` | `_OP_SQLITEEXEC` | 4512 |
| `_sqlite_close` | private | `( database-pointer -- )` | `_OP_SQLITECLOSE` | 4513 |

### MIDI

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `_pm_list` | private | `( -- )` | `_OP_PM_LIST` | 4515 |
| `_pm_open_out` | private | `( device-id -- )` | `_OP_PM_OPENOUT` | 4516 |
| `_pm_ws` | private | `( status data1 data2 -- )` | `_OP_PM_WS` | 4517 |
| `_pm_wsr` | private | `( data2 data1 status -- )` | `_OP_PM_WSR` | 4518 |
| `_pm_open_in` | private | `( device-id -- )` | `_OP_PM_OPENIN` | 4519 |
| `_pm_read` | private | `( -- data2 data1 status-or-result )` | `_OP_PM_READ` | 4520 |
| `_pm_close` | private | `( -- )` | `_OP_PM_CLOSE` | 4521 |
| `_pm_terminate` | private | `( -- )` | `_OP_PM_TERMINATE` | 4522 |

### OSC

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `_osc_put_str` | private | `( buffer offset string -- new-offset \| -1 )` | `_OP_OSC_PUT_STR` | 4524 |
| `_osc_put_i32` | private | `( buffer offset integer -- new-offset \| -1 )` | `_OP_OSC_PUT_I32` | 4525 |
| `_osc_put_f32` | private | `( buffer offset float -- new-offset \| -1 )` | `_OP_OSC_PUT_F32` | 4526 |
| `_osc_get_str` | private | `( buffer length offset -- string-pointer new-offset \| 0 -1 )` | `_OP_OSC_GET_STR` | 4527 |
| `_osc_get_i32` | private | `( buffer length offset -- value new-offset \| 0 -1 )` | `_OP_OSC_GET_I32` | 4528 |
| `_osc_get_f32` | private | `( buffer length offset -- value new-offset \| 0 -1 )` | `_OP_OSC_GET_F32` | 4529 |

### DSP

| Name | Visibility | Stack effect | Opcode | Line |
|---|---|---|---|---:|
| `_phasor` | private | `( hz sample-rate phase-base phase-count-slot -- old-phase )` | `_OP_DSP_PHASOR` | 4560 |
| `_delay` | private | `( delay-struct distance-samples -- sample )` | `_OP_DSP_DELAY` | 4531 |
| `_reverb` | private | `( left right struct cutoff feedback sample-rate -- wet-left wet-right )` | `_OP_DSP_REVERB` | 4532 |
| `_fm` | private | `( carrier-hz modulator-hz index sample-rate table table-length phase-base phase-count-slot -- sample )` | `_OP_DSP_FM` | 4563 |
| `_sineloop` | private | `( state-slot hz feedback sample-rate table table-length phase-base phase-count-slot -- sample )` | `_OP_DSP_SINELOOP` | 4564 |
| `_tri` | private | `( hz sample-rate phase-base phase-count-slot -- sample )` | `_OP_DSP_TRI` | 4565 |
| `_sawtooth` | private | `( hz sample-rate phase-base phase-count-slot -- sample )` | `_OP_DSP_SAWTOOTH` | 4566 |
| `_wavetable` | private | `( hz phase-offset sample-rate table table-length phase-base phase-count-slot -- sample )` | `_OP_DSP_WAVETABLE` | 4567 |
| `_port` | private | `( signal state-slot rise-alpha fall-alpha -- output )` | `_OP_DSP_PORT` | 4568 |
| `_fast_sin` | private | `( radians -- approximate-sine )` | `_OP_DSP_FAST_SIN` | 4538 |
| `_fast_tanh` | private | `( value -- approximate-tanh )` | `_OP_DSP_FAST_TANH` | 4539 |
| `_reverb_cells` | private | `( -- state-cells )` | `_OP_DSP_REVERB_CELLS` | 4540 |

## Remaining verification work

Registry signatures are source-reviewed, not proofs of all error paths, numerical edge cases, allocation lifetimes, or external-service behavior. Consult the C opcode body and current tests before making stronger claims.
