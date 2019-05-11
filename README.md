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

"dclang" is modelled on the grand ol' RPN calculator "dc", which is an
oft-found on a UNIX/LINUX system near you!

RPN means "Reverse Polish Notation".  That means everything uses a
'point-free-form', and there are no parenthesis, since there is a complete
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
the programmer is assumed to be a thoughtful and responsible adult.  :)
FORTH is a great language, and I mean to follow that lead.

Anyway, due to RPN, things will look like this, when you do math:

```
    4 5 + .
    9
    
    20 5 / .
    4

    1 2 3 5 + 7 16 / .s
    <4> 1 2 8 0.4375 

    # a function!
    [ testif 1 if "true" else "false" endif print cr ]
    testif
    true

    # times/again -- basic, fastest loop type, starts at zero, ascends to cutoff parameter.
    [ looptest 7 times i . again ]
    looptest
    0 1 2 3 4 5 6 7

    # for/next loop, a little slow than basic 'do', but gives step options.
    # Parameters are to/from/step.
    # Let's add the first 20 million integers!
    [ for_test 0
        20000001 1 1 for
            i +
        next . cr ]
    for_test
    200000010000000

    # this is a comment
    "This is a string!" print
    This is a string!

    # store a value at slot zero:
    227 0 !
    0 @ .
    227

    # store a value at slot 23:
    44 4 / 23 !
    23 @ .
    11

```

Notice the '.' character, which pops/prints the top-of-stack (TOS). This comes
from FORTH, as does '.s', which non-destructively shows the stack contents.
This is different from 'dc', where 'p' pops/prints the TOS.

In the looping example, the 'do/redo' block has access to a hidden variable
'i', which you can use to test conditionally and escape the loop.  This is a
weird mixture of the way FORTH uses 'do/loop' and 'begin/again'; note how
unlike traditional FORTH, you manually test the condition inside the block
yourself, just before the 'redo', which tests for a true condition before
returning to 'do'.  (I may change this in the future to be more FORTH-like,
and have separate 'do/loop' and 'begin/again' constructs.)

This project is *far* from complete, but the goal is for it to be a full-blown
Turing-complete language in the vein of FORTH.

So far, I've implemented:

  * Math:
    * +, -, *, /, %, <<, >>
    * abs, min, max, round, ceil, floor (float-versions only)
    * pow, sqrt, log, log2, log10       (float-versions only)
    * sin, cos, tan, pi, e              (float-versions only)
    * rand                              (float-versions only)
  * Logic:
    * and, or, not, xor
    * =, <>, >, <, >=, <= 
  * Stack operations:
    * drop, dup, over, swap rot, -rot, nip, tuck
    * 2drop, 2dup, 2over, 2swap, 2rot, -2rot, 2nip, 2tuck
  * Control structures:
    * if-else-endif
    * times/again & for/next (looping)
    * user-defined words (functions)
  * Strings:
    * simple string printing 
    * fancier right-justified numeric output fields
    * '#' to end-of-line' for comments
    * `uemit`, a unicode-character emitter which can help to contruct
    strings that need them.
  * Variables/Arrays:
    * ! (poke a value to a given slot, e.g. '5 32 !' puts the value 5
    into slot 32)
    * @ (peek a value, copy it to the stack, e.g. '32 @' will put our
    previously saved '5' onto the top of the stack.
    * Since the variables exist in an giant global array, there really is 
    no distinction between 'arrays' and 'variables' in dclang. Named
    variables or constants can be emulated by makings them words, e.g.:
        ```
        # make 'myvar' an alias for array slot number 53
        # N.B. this does *not* make myvar = 53; instead it give a name
        # to the slot that will hold the actual value.
        [ myvar 53 ]
        # this will store 74231 into slot 53
        74231 myvar !
        myvar @ .
        74231 
        ```

    * This works in a similar fashion for something like a string variable
    (which is, in reality an address and a length):

        ```
        [ greeting "Hello there, good people!" ]
        greeting .s
        <2> 7888448 21
        greeting print cr
        Hello there, good people!
        ```
  * Timing:
    * a clock function ('clock') so we can time execution in nanoseconds 
    for benchmarking.
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
    "test_file.txt" "w+" file-open 0 !  # save the open file ptr to slot 0
    "Some text in my file! Woo-hoo!\n"
    0 @ file-write                      # write a sentence
    0 @ file-close                      # close the file
    "test-file.txt" "r" file-open 0 !   # re-open for reading
    30 0 @ file-read print              # read 30 bytes from the file
    # will print: Some text in my file! Woo-hoo!
    0 @ file-close                      # close the file
    ```

TODO:

  * hashing/hash tables (dictionaries). I actually may forego this and look
    to hooking in sqlite3 as a general datastore instead.
  * more time functions (e.g. date, calendar stuff, etc.)
  * more string functions, as needed (basic saving and typing is all we have
  at the moment, so I mean things like splitting, searching, etc.)
  * just about everything a usuable language will need, or at least, the
  means for someone to hook C-libraries into this enchilada.
  * turtle graphics for the kids!?

There are three branches of this repo:
  * standard ('master' branch)
  * rpi-float ('rpi-flt' branch, optimized a bit for Rpi)
  * rpi-int ('rpi-int' branch, a bit more minimal, an experiment with fixed-point integers, really)

In the standard branch, everything is on the floating-point stack only at
this point.  In the `rpi-int` branch, everything is a `long int` C-type. 
There may be separate stacks for integers in the future.  Not sure if it's
necessary yet (but I am exploring the issue).

### contact

Aaron Krister Johnson

Please report bugs and successes to akjmicro@gmail.com
