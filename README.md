## dclang

### TO INSTALL:
____________

* Have the gcc compiler on your machine

```
    git clone https://github.com/akjmicro/dclang
    cd dclang
    make
    ./dclang -i examples/some_primes.dc
```

* You can also put the executable in /usr/local/bin or what-have-you.

* Experiment as you wish with compiler optimizations in the Makefile,
  particularly with float-point options, since 'dclang' is heavily
  reliant on them.

* For interaction, it's nice to use 'rlwrap' to get readline line-history:

```
    rlwrap ./dclang
```

### ABOUT:

"dclang" is an RPN, stack-based language with near-zero syntax. It is in the
spirit and tradition of FORTH and the grand ol' RPN calculator "dc", which is
oft-found on a UNIX/LINUX system near you! You can think of it as a dialect of
FORTH, much in the same way Scheme is a leaner dialect of Lisp.

RPN means "Reverse Polish Notation".  That means everything uses a
'point-free-form', and there are no parenthesis, since there is a completely
level order of operation.  Operators operate on stack operands immediately,
and leave the result on the stack immediately.  This makes the
interpreter/parser not only simple but faster than one that has to do
computational gymnastics around parsing things like braces or parenthesis,
etc.  It also saves memory, since you don't have runaway linked-list
creation that you have to later garbage-collect.  All actions happen on the
stack.  Like FORTH, this is not and never will be a garbage collected
language, but there will be operations to create variables and other data
structures like lists and hashes (dictionaries) and so on, but they will be
manually destroyed in memory to make room for other structures with other
keywords ('free').  No garbage collection means things are kept simple, and
the programmer is assumed to be a thoughtful and responsible adult. :)
FORTH is a great language, and I mean to follow that lead, even as I simplify
certain aspects of the FORTH standard in this dialect.

The trade-off for that simplicity is that one has to get used to how order of
operations work in this world (everything being immediate and w/o parenthesis).
And also, one has to get used to manipulating the stack such that defined words
make sensible, efficient use of the stack. It takes some getting used to. I direct
the user to the internet or books to search for things relating to the fine art
of programming FORTH, etc. Everything said there applies here.

Anyway, due to RPN, things will look like this, when you do math:

```
    4 5 + .
    9

    20 5 / .
    4

    0.523 sin .
    0.4994813555186418

    3 2.54 pow .
    16.28875859622752

    1 2 3 5 + 7 16 / .s
    <4> 1 2 8 0.4375

    # a function!
    : testif 1 if "true" else "false" endif print cr ;
    testif
    true

    # times/again -- basic, fastest loop type, starts at zero, ascends to cutoff parameter (minus one).
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
    4.321 3 / mynum !
    mynum @ .
    1.3773333333333333

    # low-level approach to do the same -- store a value at slot 11:
    1.15123 11 !
    0 @ .
    1.15123

```

Notice the '.' character, which pops/prints the top-of-stack (TOS). This comes
from FORTH, as does '.s', which non-destructively shows the stack contents.
This is different from 'dc', where 'p' pops/prints the TOS.

In the looping examples, the block has access to up to 3 hidden variables,
'i', 'j', and 'k' which you can use to test conditionally and escape the loop.
This allows nested loops up to three counters deep. Going any futher is a
code-smell anyway, and you should refactor to a different implementation if
you need something more.


Implemented thus far:

  * Math:
    * `+`, `-`, `*`, `/`, `%`, `<<`, `>>`
    * `abs`, `min`, `max`, `round`, `ceil`, `floor` (float-versions only)
    * `pow`, `sqrt`, `log`, `log2`, `log10`         (float-versions only)
    * `sin`, `cos`, `tan`, `pi`, `e`                (float-versions only)
    * `rand`                                        (float-versions only)
  * Logic:
    * `and`, `or`, `not`, `xor`
    * `=`, `<>`, `>`, `<`, `>=`, `<=`
  * Stack operations:
    * `drop`, `dup`, `over`, `swap`, `rot`, `-rot`, `nip`, `tuck`, `pick`
    * `2drop`, `2dup`, `2over`, `2swap`, `2rot`, `-2rot`, `2nip`, `2tuck`
  * Control structures:
    * `if`-`else`-`endif`
    * `times`/`again`; `for`/`next` (looping)
    * user-defined words (functions)
  * Strings:
    * simple string printing w/ `print`
    * fancier right-justified numeric output fields: `.rj`
    * `strcat`, `strcpy`, `strtok`, `memcpy`
    * strong comparison with: `strlen`, `str=`, `str<`, `str>`
    * find a substring with `strfind`
    * '#' to end-of-line' for comments
    * `uemit`, a unicode-character emitter which can help to contruct
    strings that need them.
    * convert character bytes to equivalent numerical value with `ord`
    * convert integers to hex-string with `tohex`.
  * Variables/Arrays:
    * Declare a constant with `const`:
        ```
        1 pi 2 * / const INV2PI
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
        # that the variable owns 16 slots, making it an array. You
        # are responsible for knowing the bounds of the array yourself.
        # there are no protections keeping you from writing into neighboring
        # cells:
        var myarr 16 allot
        ```
        There is also `create`, which does something similar, but is paired typically
        with `,` which is an operator to place a stack value immediately into a storage
        location. So, to initialize an array of four values to `1`, you'd do:
        ```
        create myarr 1 , 1 , 1 , 1 ,
        ```
        Note that the comma operator is an actual operator-word, it's not a delimiter!
    * `!` (poke a value to a given slot, e.g. `5 funvar !` puts the value 5
      into `funvar`)
    * `@` (peek a value, copy it to the stack, e.g. `funvar @` will put our
    previously saved '5' onto the top of the stack.
    * Since the variables exist in an giant global array, there really is
    no distinction between 'arrays' and 'variables' in dclang. Named
    variables or constants can be emulated by makings them words, e.g.:
        ```
        # make 'myvar' an alias for array slot number 53
        # N.B. this does *not* make myvar = 53; instead it give a name
        # to the slot that will hold the actual value.
        : myvar 53 ;
        # this will store 7.4231 into slot 53
        7.4231 myvar !
        myvar @ .
        7.4231
        ```

    * This works in a similar fashion for something like a string variable
    (which is, in reality an address and a length):
      ```
      : greeting "Hello there, good people!" ;
      greeting .s
      <1> 94123539921536
      greeting print cr
       Hello there, good people!
      ```
  * A global hash table (string keys and string values only). This is in the spirit of `redis`, in a way:
    ```
    "some value" "mykey" h!
    "mykey" h@ print cr
    some value
    ```
  * Timing:
    * a clock function ('clock') so we can time execution in nanoseconds for benchmarking.
    * A hook into CPU-cycle clock, called 'rdtsc'. (not available on RPi)
    * A sleep function (C's `nanosleep` under-the-hood)

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
    myfile @ fwrite                       # write a sentence
    myfile @ fclose                       # close the file
    "test-file.txt" "r" fopen myfile !    # re-open for reading
    30 myfile @ fread print               # read 30 bytes from the file
    # will print: Some text in my file! Woo-hoo!
    myfile @ fclose                       # close the file
    ```

TODO:

  * More time functions (e.g. date, calendar stuff, etc.)
  * More string functions, as needed. We have most of the basic `string.h` C-level functions. What could be implemented
  would be things like search/replace and `regex`.
  * The `dsp.dc` lib has a good start! But it is growing still...this is an active
    area for my interests!
  * A socket layer for networking would be nice.
  * More OS-level integration (file system interaction, directories, permissions, etc.)
  * Turtle graphics for the kids!?

There are three branches of this repo:
  * Standard ('master' branch)
  * (not active for a while) rpi-float ('rpi-flt' branch, optimized a bit for Rpi)
  * (almost dead?) rpi-int ('rpi-int' branch, a bit more minimal, an experiment with fixed-point integers, really)

(Quasi-deprecated note about branches):
In the standard branch, everything is on the floating-point stack only at
this point.  In the `rpi-int` branch, everything is a `long int` C-type.
There may be separate stacks for integers in the future.  Not sure if it's
necessary yet (but I am exploring the issue).

### contact

Aaron Krister Johnson

Please report bugs and successes to akjmicro@gmail.com
