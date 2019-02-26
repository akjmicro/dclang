## dclang

### TO INSTALL:
____________

* Have the gcc compiler on your machine

```
    git clone https://github.com/akjmicro/dclang
    cd dclang
    make
    ./dclang < examples/some_primes.dc
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

    0.523 sin .
    0.4994813555186418

    3 2.54 pow .
    16.28875859622752

    1 2 3 5 + 7 16 / .s
    <4> 1 2 8 0.4375 

    # a function!
    [ testskip 1 2 4 skip 0 0 0 0 3 4 . ]
    testskip
    1 2 3 4

    # do/redo -- basic, starts at zero, ascends, test is 'manual' at end
    [ looptest do i . i 7 < redo ]
    looptest
    0 1 2 3 4 5 6 7

    # for/next loop, more than twice as fast. Parameters are to/from/step.
    # Let's add the first 20 million integers!
    [ for_test 0
        20000001 1 1 for
            i +
        next . cr ]
    for_test
    200000010000000

    # this is a comment
    s" This is a string!" print
    This is a string!

    # store a value at slot zero:
    1.15123 0 !
    0 @ .
    1.15123

    # store a value at slot 23:
    4.132 3 / 23 !
    23 @ .
    1.3773333333333333

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
    * abs, min, max, round, ceil, floor
    * pow, sqrt, log, log2, log10
    * sin, cos, tan, pi, e
  * Logic:
    * and, or, not, xor
    * =, <>, >, <, >=, <= 
  * Stack operations:
    * drop, dup, swap, over, rot, -rot, nip, tuck
    * 2drop, 2dup, 2swap, 2over, 2rot, -2rot, 2nip, 2tuck
  * Control structures:
    * skip (a kind of jumping mechanism that replaces if/else/endif)
    * for/next & do/redo (looping)
    * more performant 'quick' do/redo: `N qdo ... qredo`
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
        [ myvar 53 ]
        # this will store 7.4231 into slot 53
        7.4231 myvar !
        myvar @ .
        7.4231 
        ```

    * This works in a similar fashion for something like a string variable
    (which is, in reality an address and a length):

        ```
        [ greeting s"Hello there, good people!" ]
        greeting .s
        <2> 7888448 21
        greeting print cr
        Hello there, good people!
        ```
  * Timing:
    * a clock function ('clock') so we can time execution in nanoseconds 
    for benchmarking.
    * A hook into CPU-cycle clock, called 'rdtsc'.
    * A sleep function (C's `nanosleep` under-the-hood)

  * Importing a file of dclang code:
    * From the interpreter
        ```
        s" examples/some_primes.dc" import
        ```
    * On the command-line, then drop to interpreter:
        ```
        ./dclang -i examples/some_primes.dc
        ```

TODO:

  * open/read/write/close to the filesystem (importing code is implemented)
  * hashing/hash tables (dictionaries)
  * more time functions (e.g. date, calendar stuff, etc.)
  * more string functions, as needed (basic saving and typing is all we have
  at the moment, so I mean things like splitting, searching, etc.)
  * just about everything a usuable language will need, or at least, the
  means for someone to hook C-libraries into this enchilada.
  * turtle graphics for the kids!

Everything is on the float-point stack only at this point. There may be
separate stacks for integers in the future. Not sure if it's necessary.

### contact

Aaron Krister Johnson

Please report bugs and successes to akjmicro@gmail.com
