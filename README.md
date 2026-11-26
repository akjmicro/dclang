## dclang

### TO INSTALL AND SETUP:
____________

* Have the gcc (Linux) or clang (Mac) compiler on your machine.

* Install the PortMidi and SQLite 3 development packages. The build needs
  `portmidi.h`, `porttime.h`, and `sqlite3.h`, plus their corresponding
  libraries.

* Clone the repo

  ```
  # clone the repo and build
  git clone https://github.com/akjmicro/dclang
  cd dclang
  ```

* Copy the appropriate `Makefile.[linux|mac]` based on your OS to `Makefile`

  ```
  # Linux
  cp Makefile.linux Makefile

  # MacOS
  cp Makefile.mac Makefile
  ```

* Build, using the new `Makefile`

  ```
  # build
  sudo make install
  ```

* `make install` puts the executable `dclang` into `/usr/local/bin` by default.

* You'll need to set `DCLANG_LIBS` to the location of your dclang source folder.
  ```
  export DCLANG_LIBS=/<this>/<source>/<folder>/<location>/lib
  ```

  If you use the `Makefile` directive `make install`, it will link
  the libraries at `/usr/local/lib/dclang`, so you'd do:
  ```
  export DCLANG_LIBS=/usr/local/lib/dclang
  ```

  You can add this export statement to your shell (`bash`, `zsh`, etc.) startup script, of course.

* Experiment as you wish with compiler optimizations in the Makefile,
  particularly for numeric and DSP-heavy programs.

* Run an example to test the interpreter

  ```
  # try running the primes example:
  dclang examples/some_primes.dc
  ```

* For interaction, it's nice to use 'rlwrap' to get readline line-history:
  ```
  rlwrap ./dclang
  ```
  One can also create an alias to `dclang` that uses `rlwrap`:
  ```
  alias dclang='rlwrap dclang'
  ```

* Interested in using an RPN language for MIDI coding? For MIDI instructions,
  check out the details in `examples/midi/README.md`

### ABOUT:

#### What's new in major version 7?

Version 7 is an intentional, breaking change to dclang's numeric model. The
interpreter's stack, variables, constants, locals, and instruction parameters
now use pointer-sized, untagged `uintptr_t` cells instead of storing every
value as `DCLANG_FLT`. On a 64-bit target one cell can hold a `double`; on a
32-bit target, including the intended Bela configuration, one cell can hold a
`float`. This removes constant conversion through `double` for integer,
address, counter, and raw-cell work, while retaining one shared stack.

The main source-level changes are:

* Integer operations retain the familiar spellings: `+`, `-`, `*`, `/`, `%`,
  `abs`, `min`, `max`, and the ordinary comparison words. `*/` computes a
  signed multiply-then-divide with a double-width intermediate product. `/%`
  and `*/%` additionally return quotient followed by remainder, leaving the
  remainder on top in the same order as dclang's library `divmod`.
* Floating-point operations now say so: `f+`, `f-`, `f*`, `f/`, `f%`,
  `fsqrt`, `fpow`, `fsin`, `fpi`, `frand`, `f=`, and their related `f...`
  words. Formatting words are now `f.`, `f.rj`, and `f.pz`.
* `i>f` and `f>i` perform numeric conversion. Merely applying a different
  operator reinterprets the same raw bits; it does not convert them.
* A numeric token containing a decimal point or decimal exponent is parsed as
  floating point. Other numeric tokens are parsed as integers. Thus `1` and
  `1.0` intentionally have different cell representations.
* `.`, `u.`, and `f.` print signed-integer, unsigned-integer, and floating-point
  interpretations; `..`, `u..`, and `f..` omit the trailing space. `.rj`,
  `u.rj`, and `f.rj` provide right-justified fields. `.s` now displays
  non-destructive, type-neutral raw hex.
* `sortnums` has a signed-integer contract and `fsortnums` has a packed-float
  contract. Mixed arrays remain the caller's responsibility.

This is a major API jump rather than a compatibility-alias release. Version 6
programs using unprefixed floating-point words need to be migrated. The bundled
standard libraries, tests, and tracked examples have been migrated with the
interpreter. `dclang2c-dsp` now emits the same raw-cell model and can target either
32-bit/float or 64-bit/double cells as described near the end of this document.

`dclang` is an RPN, stack-based language with near-zero syntax. It is in
the spirit and tradition of `forth` and the grand ol' RPN calculator
`dc`, which is oft-found on a UNIX/LINUX system near you! You can think
of it as a dialect of `forth`, much in the same way `scheme` is a leaner
dialect of `lisp`. Why `dclang` and not `gforth`? For the same reasons
one would choose `scheme` instead of `lisp`! Smaller, easier to learn,
in some ways, better in terms of usability and syntactical and naming
improvements. I wanted to take what I like about `forth`, shave off what
I didn't like, and make a more user-friendly idealized version of
`forth` -- one that `forth` folks would recognize, but also would
perhaps be friendlier to new users.

There are two constant goals of `dclang`:

  1) to present hackers with a USABLE tool that they will enjoy!
  2) to create a lean, performant tool that utterly smokes most
     interpreted languages.

I do not want to get stuck in exploring CS theory (although that is
respectable and interesting) so much that I have a "Turing Tarpit Tool"
that does nothing. `dclang` is slowly gathering features that means you
can use it like you'd use `python`, `bash`, `gforth`, etc...and I have
an eye to be guided by some of the key "daily use" functionality that is
for instance offered by `glibc` in C. In fact, you might say that I'll
know `dclang` is really done when every (or almost every) aspect/feature
of `glibc` is somehow reflected in the in-built capabilities of
`dclang`.

RPN means "Reverse Polish Notation".  That means everything uses a
'point-free-form', and there are no parenthesis, since there is a
completely level order of operation.  Words operate on stack operands
immediately, and leave the result on the stack immediately.  This makes
the interpreter/parser not only simple but faster than one that has to
do computational gymnastics around parsing things like braces or
parenthesis, etc.  It also saves memory, since you don't have runaway
linked-list creation that you have to later garbage-collect.  All
actions are coordinated through the stack. Like `forth`, this is not and never
will be a garbage collected language. Variables use a fixed cell arena;
strings and buffers use explicit pool-management words such as `free`, `mark`,
and `rewind`; lists and trees have their own deletion words. No garbage
collection means things are kept simple, and the programmer is assumed
to be a thoughtful and responsible adult. :) `forth` is a great
language, and I mean to follow that lead, even as I simplify certain
aspects of the `forth` standard in this dialect.

The trade-off for that simplicity is that one has to get used to how
order of operations work in this world (everything being immediate and
w/o parenthesis). And also, one has to get used to manipulating the
stack such that defined words make sensible, efficient use of the stack.
It takes some getting used to. I direct the user to the internet or
books to search for things relating to the fine art of stack programming.
Many Forth techniques are useful here, but dclang's syntax, words, stack
effects, and memory model must be checked on their own terms.

Anyway, due to RPN, things will look like this, when you do math:

The stack and variable memory contain untagged, pointer-sized cells. Integer
and floating-point values can share the same stack, but the operator you call
determines how each cell's bits are interpreted. Integer math uses the familiar
operators; floating-point math uses `f`-prefixed operators. `i>f` and `f>i`
perform numeric conversion rather than merely reinterpreting bits.
Numeric literals containing a decimal point or decimal exponent (`e`/`E`) are
packed as floating-point values. Other numeric literals are cells containing
integers; hexadecimal integer notation is supported. The same rule applies to
`tonum`. Cells do not carry runtime type tags, so choosing sensible operators
for the data is deliberately the programmer's responsibility.
The floating-point output below is from a 64-bit build, where `DCLANG_FLT` is
`double`; 32-bit builds use `float` and naturally show different precision.

```
    4 5 + .
    9

    20 5 / .
    4

    0.523 fsin f.
    0.4994813555186418119

    3 i>f 2.54 fpow f.
    16.28875859622752031

    1 2 3 5 + 7 16 / .s
    data_stack: <4> 0x0000000000000001 0x0000000000000002 0x0000000000000008 0x0000000000000000
    save_stack: <0>

    # a function!
    : testif 1 if "true" else "false" endif print cr ;
    testif
    true

    # times/again -- basic counted loop, starts at zero, ascends to cutoff parameter (minus one).
    : looptest 7 times i . again ;
    looptest
    0 1 2 3 4 5 6

    # for/next loop, a little slower than basic 'times/again', but gives step options.
    # Parameters are to/from/step.
    # Let's add the first 20 million integers!
    : for_test 0
        20000001 1 1 for
            i +
        next . cr ;
    for_test
    200000010000000

    # this is a comment
    "This is a string!" print
    This is a string!

    # create a variable and store a value at it:
    var mynum
    4.321 3 i>f f/ mynum !
    mynum @ f.
    1.440333333333333243

    # low-level approach to do the same -- store a value at slot 11:
    1.15123 11 !
    11 @ f.
    1.151229999999999976

```

Notice the `.` character, which pops and prints the top-of-stack (TOS) as a
signed integer. This comes from `forth`, as does `.s`, which non-destructively shows the
stack contents as fixed-width raw hexadecimal cells. Use `u.` or `f.`
to pop and print an unsigned-integer or floating-point interpretation.
The hexadecimal width is 16 digits on a 64-bit build and 8 on a 32-bit build.
This is different from `dc`, where `p` pops/prints the TOS.

In the looping examples, the block has access to up to 3 hidden
variables, 'i', 'j', and 'k' which you can use to test conditionally and
escape the loop. This allows nested loops up to three counters deep.
Going any further is a code-smell anyway, and you should refactor to a
different implementation if you need something more.

The user is encouraged to peruse the `examples/` and `tests/` directories
to get a feel for the style and syntax of `dclang`. When you first
launch an interactive commandline session by typing `dclang` (or `rlwrap
dclang` to enable command history, if you have `rlwrap`), you will see a
categorized listing of all the primitives. You can always see the list
again by calling the `primitives` word, which will show the info listing
again.

A non-exhaustive notes on some of the things implemented thus far:

  * Math:
    * signed integer arithmetic: `+`, `-`, `*`, `/`, `%`, `/%`, `*/`, `*/%`, `abs`, `min`, `max`
    * raw-cell bit operations: `and`, `or`, `not`, `xor`, `<<`, `>>`
    * float arithmetic: `f+`, `f-`, `f*`, `f/`, `f%`, `fabs`, `fmin`, `fmax`
    * float-to-integer rounding: `fround`, `fceil`, `ffloor`
    * higher math: `fpow`, `fsqrt`, `fexp`, `flog`, `flog2`, `flog10`, `fe`
    * trigonometry: `fsin`, `fcos`, `ftan`, `fasin`, `facos`, `fatan`,
      `fatan2`, `fsinh`, `fcosh`, `ftanh`, `fpi`
    * random float: `frand`
    * numeric conversion: `i>f`, `f>i`
  * Logic:
    * signed integer comparisons: `=`, `<>`, `>`, `<`, `>=`, `<=`
    * float comparisons: `f=`, `f<>`, `f>`, `f<`, `f>=`, `f<=`
  * Stack operations:
    * `drop`, `dup`, `over`, `swap`, `pick`, `2drop`, `2dup`, `2over`
    * `svpush`, `svpop`, `svdrop`, `svpick`
    * `depth`, `clear`, `svdepth`, `svclear`
  * Control structures:
    * `if`-`else`-`endif`
    * `times`/`again`; `for`/`next` (looping)
    * user-defined words (functions)
  * Strings:
    * simple string printing w/ `print`
    * fancier floating-point output fields: `f.rj`, `f.pz`
    * `strtok`, `mempcpy`, `memset`, `mkbuf`, `free`
    * strong comparison with: `strlen`, `str=`, `str<`, `str>`
    * find a substring with `strfind`
    * '#' to end-of-line for comments
    * `uemit`, a unicode-character emitter which can help to construct
    strings that need them.
    * convert character bytes to equivalent numerical value with `ord`
    * convert integers to hex-string with `tohex`.
    * `isalnum`, `isalpha`, `iscntrl`, `isdigit`, `isgraph`, `islower`, `isprint`
      `ispunct`, `isspace`, `isupper`, `isxdigit` -- all of these can take the integer output
      from `ord` and return `-1` (true) or `0` (false) for determining the class of a given character.
      (N.B.: If given a string of len > 1, `ord` uses the first character of the string by default.)
    * a `regex.dc` library, which uses the private regex primitives: `_regcomp`, `_regexec`, and `_regread`
      in a `match` word, giving basic regex functionality.
  * Variables/Arrays:
    * Declare a constant with `const`:
        ```
        1.0 fpi 2.0 f* f/ const INV2PI
        ```
    * Declare a variable with `var`:
        ```
        var myvar
        ```
        or
        ```
        # This declares _and_ initializes:
        var myvar 42 myvar !
        ```
        or
        ```
        # Declare a variable and advance the variable pointer such
        # that the variable owns 16 slots total, making it an array. You
        # are responsible for knowing the bounds of the array yourself.
        # there are no protections keeping you from writing into neighboring
        # cells:
        var myarr 16 allot
        ```

        There is also `create`, which does something similar, but is
        paired typically with `,` which is an operator to place a stack
        value immediately into a storage location. So, to initialize an
        array of four values to `1`, you'd do:
        ```
        create myarr 1 , 1 , 1 , 1 ,
        ```

        Note that the comma operator is an actual operator-word, it's
        not a delimiter!

    * `!` (poke a value to a given slot, e.g. `5 funvar !` puts the value 5
      into `funvar`)
    * `@` (peek a value, copy it to the stack, e.g. `funvar @` will put our
    previously saved '5' onto the top of the stack.
    * Since the variables exist in a giant global array, there really is
    no distinction between 'arrays' and 'variables' in dclang.
    Arrays are 0-based:
        ```
        var myarray 4 allot

        # set slot 0 (first slot)
        3 myarray !
        # set slot 1 (2nd slot)
        5 myarray 1 + !
        # set slot 2 (3rd slot)
        7 myarray 2 + !
        # set slot 3 (4th slot)
        11 myarray 3 + !

        # get slot 2 (3rd slot):
        myarray 2 + @  . cr
        7   # result shown

        # Alternatively, declare the 4-slot array in one go:
        create myarray 3 , 5 , 7 , 11 ,

        # get slot 3 (4th slot) this time:
        myarray 3 + @  . cr
        11  # result shown
        ```

    * `sortnums` sorts an array as signed integers, `fsortnums` sorts it as
      packed floating-point values (with NaNs last), and `sortstrs` sorts
      string addresses by string contents. These operations do not detect or
      convert mixed numeric arrays.

    * This works in a similar fashion for a string value, which is a cell
    containing the address of null-terminated string storage:
      ```
      : greeting "Hello there, good people!" ;
      greeting .s
      data_stack: <1> 0x0000...   # the exact address varies by process
      save_stack: <0>
      greeting print cr
      Hello there, good people!
      ```
  * A global hash table with string keys and raw-cell values. String values
    work naturally because their addresses fit in cells. This is in the spirit
    of `redis`, in a way:
    ```
    "some value" "mykey" h!
    "mykey" h@ print cr
    some value
    ```
  * Tree-based key/value stores, similar to the hash above, but access is a slightly slower
    (won't be very noticeable in most use-cases) `O(log n)` access time. Based on the `tsearch` `glibc` functions:
    ```
    tmake const :mytree            # make a tree, put it in the constant :mytree
    :mytree "foo" "bar" t!         # Usage: <tree> <key> <val> t! (tree! sets a value on <key>, on <tree>)
    :mytree "foo" t@               # Usage: <tree> <key> t@       (tree@ gets a value from <key>, on <tree>)
    cr print cr                    # Let's print the output!
    bar                            # <-- t@ output

    cr :mytree twalk               # walk the tree with treewalk

    key=foo, value=bar             # <-- twalk output, note the line break via `cr`

    :mytree "favorite ice cream flavor" "Pralines & Cream" t!
    :mytree twalk                  # walk the tree again; see new values

    key=foo, value=bar
    key=favorite ice cream flavor, value=Pralines & Cream

    :mytree "foo" tdel             # delete a key
    :mytree twalk

    key=favorite ice cream flavor, value=Pralines & Cream
    ```

  * Linked lists: `lmake`, `lpush`, `lpop`, `l!`, `l@`, `lins`, `lrem`,
    `lsize`, `ldel`
  * Timing:
    * `clock` returns absolute wall time as two integer cells:
      `( epoch-seconds nanoseconds )`. The musical timing libraries hide this
      representation behind `synchronized_start`, `set_clock_start`, and the
      gate words.
    * `monoclock` returns floating-point seconds elapsed since dclang started;
      use it for benchmarks and local elapsed-time measurements.
    * `epoch` returns integer seconds since the Unix epoch.
    * `sleep` uses C's `nanosleep` under the hood.
  * Importing a file of dclang code:
    * From the interpreter
        ```
        "examples/some_primes.dc" import
        ```
    * On the command-line, then drop to interpreter:
        ```
        ./dclang -i examples/some_primes.dc
        ```
  * Read/write of file:
    ```
    var myfile
    "test_file.txt" "w+" fopen myfile !  # save the open file ptr to a var slot
    "Some text in my file! Woo-hoo!\n"
    dup strlen myfile @ fwrite            # write a sentence, specifying its length, to the file
    myfile @ fclose                       # close the file
    "test_file.txt" "r" fopen myfile !    # re-open for reading
    var buf 1024 mkbuf buf !              # create a memory buffer
    buf @ 30 myfile @ fread               # read 30 bytes from file, put in 'buf'
    .                                     # print num bytes read
    # will print: Some text in my file! Woo-hoo!
    buf @ print cr
    myfile @ fclose                       # close the file
    ```

  * `tcplisten`, `tcpaccept` for server primitives, `tcpconnect` for
    clients. See the examples directory.


### A note on "composibility"

After wrestling a bit back-n-forth with the stack API for trees and
lists, I opted for what what some might consider a non-standard approach
to stack order for both. On the face of it, the approach for these
structures is inconsistent with the other variable types, "normal array
slots" and "hashes". Consider:

```
var thing
5 thing !         # normal 'FORTHish setter': <value> <slot> <action>
  thing @         # normal 'FORTHish getter':         <slot> <action>

# and for hashes:

"bar" "foo" h!
      "foo" h@
```

So, why have I not preserved this "standard stack-order way" for trees
and lists? Well for starters, I did. However, since these structures can
sometimes be nested, and one can do neat things with nesting, keeping
this strictly FORTH-like way can be a nuisance, and unwieldy. Take for
instance, a nested list:

```
# make 3 lists
var outer_list
lmake outer_list !
var inner_list1
lmake inner_list1 !
var inner_list2
lmake inner_list2 !

# If we use a FORTH-ish stack order:
inner_list1 outer_list lpush
inner_list2 outer_list lpush
2 inner_list1 lpush
3 inner_list1 lpush
5 inner_list2 lpush
7 inner_list2 lpush
0 0 outer_list l@ l@     # 2
1 0 outer_list l@ l@     # 3
0 1 outer_list l@ l@     # 5
1 1 outer_list l@ l@     # 7

# If we use a "un FORTH-ish" composeable stack order:
outer_list inner_list1 lpush
outer_list inner_list2 lpush
inner_list1 2 lpush
inner_list1 3 lpush
inner_list2 5 lpush
inner_list2 7 lpush
outer_list 0 l@ 0 l@    # 2
outer_list 0 l@ 1 l@    # 3
outer_list 1 l@ 0 l@    # 5
outer_list 1 l@ 1 l@    # 7
```

Now, isn't the 2nd way much nicer, and easier to parse? For starters, we
preserve the "odometer" feeling of digging into a nested structure that
most folks are used to. Also, notice how, related to that, the idea of
"outer" and "inner" is actually preserved in a left-to-right reading.
Finally, when you de-reference the `outer_list` with `l@`, you get the
inner list, and can further deference _that_ in a "chaining" fashion
with yet another index and `l@` operator. I find this much clearer,
cleaner, and more intuitive.

Since trees can be nested in a similar way, I changed their stack-order
expectations in a similar way. There may come a time where I consider
doing this to normal variables and hashes -- however, I've written so
much code, and a chain of dereferences making for strange stack-order
(i.e. nesting) is less common in those cases, and might be more
upsetting the apple cart at this point. More thought and consideration
of the matter is required at this point to convince me that changing
that is worth it.

### Contact

Aaron Krister Johnson

Please report bugs and successes to <akjmicro@gmail.com>

### Compiling a DSP word to C

`dclang2c-dsp` is an experimental host-side patch compiler for a bounded stereo DSP
entry word with stack effect `( -- left right )`. Build it with the same
dependencies as dclang. It is deliberately not a general-purpose dclang-to-C
compiler: its supported execution regime and real-time guardrails are part of
its contract.

Generated programs use raw `uintptr_t` stack, local, and memory cells and
explicitly pack and unpack their platform floating type. Translation defaults
to the host cell width. Set `DCLANG2C_CELL_BITS=32` when generating for Bela;
the generated source contains a compile-time width guard so a mismatched target
fails rather than silently using an incorrect state layout. `make bela-dsp`
sets the 32-bit target automatically.

```sh
make -f Makefile.linux dclang2c-dsp   # use Makefile.mac on macOS
DCLANG_LIBS="$PWD/lib" SR=44100 \
  ./dclang2c-dsp instrument.dc render_frame instrument.c
cc -O3 -ffp-contract=off -I"$PWD" -c instrument.c
```

The direct command defaults to the host width. For a manually managed Bela
build, add `DCLANG2C_CELL_BITS=32` during translation and compile the resulting
source on Bela. The `make bela-dsp` target performs that selection for you.
That target also enables Bela's `math_neon` library for supported generated
scalar math; override `BELA_LIBRARIES` and `BELA_CPPFLAGS` to opt out.

Generated programs expose block-boundary control accessors:
`dclang2c_control_index`, `dclang2c_get_u32`, `dclang2c_get_float`,
`dclang2c_set_u32`, and `dclang2c_set_float`. Resolve indexes during setup and
call setters only from the audio thread between render blocks. An auxiliary
receiver thread may publish incoming values through atomics, but it must not
write generated DSP memory concurrently with `dclang2c_render`.

The generic OSC control wrapper demonstrates this division. From the DSP
examples directory, generate and build the drum-machine Bela project with:

```sh
./run-bela-example.sh drum_machine.dc
```

The launcher accepts any migrated DSP example and an optional entry-word. It
uses the ordinary Bela callback by default. When an instrument has a companion
`<instrument>_controller.dc`, it automatically selects the generic OSC control
wrapper. `BELA_HOST`, `BELA_PROJECT`, `BELA_RENDER`, and `SR` may be supplied as
environment overrides. It invokes the repository's `bela-dsp` target and
prints commands for starting the resulting executable from an interactive Bela
shell. Deployment does not start or stop the project process automatically.

Then run the `/tmp/keys` bridge on the laptop:

```sh
BELA_HOST=192.168.7.2 dclang \
  examples/dsp_examples/drum_machine_controller.dc
```

The bridge polls the existing drum key files at 50 Hz. Controllers use the
shared `dclang_control_f` and `dclang_control_i` words to send generated variable
names and typed values to `/dclang/set` over UDP port 7562. Instrument-specific
key names and mappings remain in the controller; the Bela wrapper contains no
instrument-specific control table.

The blocking receiver pthread resolves each variable with
`dclang2c_control_index` and publishes updates through a bounded lock-free
queue. The real-time callback applies them only between audio blocks. It emits
silence and does not advance generated DSP state until the first valid control
update has been applied, so starting a companion controller also starts the
instrument from its initial sequence state.

On 32-bit targets the interpreter uses the native `float` libm entry points.
Bela builds may additionally define `DCLANG_MATH_NEON` and link
`-lmathneon` to select Bela's faster approximate NEON backend. This remains
opt-in because its supported ranges and numerical results differ from libm.

`dclang2c-dsp` evaluates the patch setup at compile time. Use a definition-only audio
entry point and omit any top-level playback command. Call
`void dclang2c_init(void)` once, then call
`void dclang2c_render(float *output, size_t frames)` to fill a caller-owned
buffer of `2 * frames` interleaved samples. `float` is the default render API
for Bela; desktop users may compile generated code with
`-DDCLANG2C_SAMPLE_TYPE=double` and declare a matching `double *` caller API.
Link the generated code with `-lm`. Copy `dclang_dsp.h`, `dclang_osc.h`, `dclang_fast_math.h`, and
`dclang_phiverb.h` with the generated source when building on another machine. The generated program does not link against
`libdclang`.

`dclang_phiverb.h` is generated from the Faust library rather than maintained
by hand. After editing `phiverb.lib`, regenerate the integration header with:

```sh
python3 contrib/generate_phiverb.py /path/to/phiverb.lib
```

The current subset supports statically resolved calls, locals, balanced
conditionals, explicitly typed scalar math, variable reads and writes, and the native
oscillator, port, delay, and reverb kernels. Loops, dynamic execution, audio
I/O inside the entry word, and unsupported primitives produce a compile-time
diagnostic. The frontend evaluates setup, so only compile trusted inputs.
The sample rate is fixed at compilation, the generated state is single-instance, and
patches that use `frand` require matching host and target C-library random
sequences for exact reproducibility.
