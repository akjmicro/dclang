## dclang

### TO INSTALL:
____________

* Have the gcc compiler on your machine:

  ```
  git clone https://github.com/akjmicro/dclang
  cd dclang
  make
  ./dclang -i examples/some_primes.dc
  ```

* You can also put the executable in `/usr/local/bin` or what-have-you.
  `make install` should be able to do this for you on say, Linux or MacOSX.

* Experiment as you wish with compiler optimizations in the Makefile,
  particularly with float-point options, since 'dclang' is heavily
  reliant on them.

* For interaction, it's nice to use 'rlwrap' to get readline line-history:
  ```
  rlwrap ./dclang
  ```
  One can also create an alias to `dclang` that uses `rlwrap`:
  ```
  alias dclang='rlwrap dclang`
  ```

### ABOUT:

`dclang` is an RPN, stack-based language with near-zero syntax. It is in the
spirit and tradition of `forth` and the grand ol' RPN calculator `dc`, which is
oft-found on a UNIX/LINUX system near you! You can think of it as a dialect of
`forth`, much in the same way `scheme` is a leaner dialect of `lisp`. Why `dclang`
and not `gforth`? For the same reasons one would choose `scheme` instead of `lisp`!
Smaller, easier to learn, in some ways, better in terms of usability and syntactical
and naming improvements. I wanted to take what I like about `forth`, shave off what I
didn't like, and make a more user-friendly idealized version of `forth` -- one that `forth`
folks would recognize, but also would perhaps be friendlier to new users.

There are two constant goals of `dclang`: 

  1) to present hackers with a USABLE tool that they will enjoy!
  2) to create a lean, performant tool that utterly smokes most
     interpreted languages.

I do not want to get stuck in exploring CS theory (although that
is respectable and interesting) so much that I have a "Turing Tarpit Tool" that
does nothing. `dclang` is slowly gathering features that means you can use it like
you'd use `python`, `bash`, `gforth`, etc...and I have an eye to be guided by some
of the key "daily use" functionality that is for instance offered by `glibc` in C.
In fact, you might say that I'll know `dclang` is really done when every (or almost every)
aspect/feature of `glibc` is somehow reflected in the in-built capabilities of `dclang`.

RPN means "Reverse Polish Notation".  That means everything uses a
'point-free-form', and there are no parenthesis, since there is a completely
level order of operation.  Words operate on stack operands immediately,
and leave the result on the stack immediately.  This makes the
interpreter/parser not only simple but faster than one that has to do
computational gymnastics around parsing things like braces or parenthesis,
etc.  It also saves memory, since you don't have runaway linked-list
creation that you have to later garbage-collect.  All actions happen on the
stack.  Like `forth`, this is not and never will be a garbage collected
language, but there will be operations to create variables and other data
structures like lists and hashes (dictionaries) and so on, but they will be
manually destroyed in memory to make room for other structures with other
keywords ('free').  No garbage collection means things are kept simple, and
the programmer is assumed to be a thoughtful and responsible adult. :)
`forth` is a great language, and I mean to follow that lead, even as I simplify
certain aspects of the `forth` standard in this dialect.

The trade-off for that simplicity is that one has to get used to how order of
operations work in this world (everything being immediate and w/o parenthesis).
And also, one has to get used to manipulating the stack such that defined words
make sensible, efficient use of the stack. It takes some getting used to. I direct
the user to the internet or books to search for things relating to the fine art
of programming `forth`, etc. Everything said there applies here.

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
from `forth`, as does '.s', which non-destructively shows the stack contents.
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
    * `drop`, `dup`, `over`, `swap`, `pick`, `2drop`, `2dup`, `2over`
    * `svpush`, `svpop`, `svdrop`, `svpick`
    * `depth`, `clear`, `svdepth`, `svclear`
  * Control structures:
    * `if`-`else`-`endif`
    * `times`/`again`; `for`/`next` (looping)
    * user-defined words (functions)
  * Strings:
    * simple string printing w/ `print`
    * fancier right-justified numeric output fields: `.rj`
    * `strtok`, `mempcpy`, `memset`, `mkbuf`, `free`
    * strong comparison with: `strlen`, `str=`, `str<`, `str>`
    * find a substring with `strfind`
    * '#' to end-of-line for comments
    * `uemit`, a unicode-character emitter which can help to contruct
    strings that need them.
    * convert character bytes to equivalent numerical value with `ord`
    * convert integers to hex-string with `tohex`.
    * `isalnum`, `isalpha`, `iscntrl`, `isdigit`, `isgraph`, `islower`, `isprint`
      `ispunct`, `isspace`, `isupper`, `isxdigit` -- all of these can take the integer output
      from `ord` and return `1` (true) or `0` (false) for determinng the class of a given character.
      (N.B.: If given a string of len > 1, `ord` uses the first character of the string by default.)
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
  * Private tree-based key/value stores, similar to the hash above, but access is a slightly slower
    (won't be very noticeable in most use-cases) `O(log n)` access time. Based on the `tsearch` `glibc` functions:
    ```
    var :mytree                  # sets up a variable to store our tree data
    treemake :mytree !           # make a tree, put it on the variable :mytree that we made
    "bar" "foo" :mytree @ tree!  # Usage: <value> <key> <which-tree> tree! (tree! sets a value on <key>, on <which-tree>)
    "foo" :mytree @ tree@        # Usage: <key> <which-tree> tree@         (tree@ gets a value from <key>, on <which-tree>) 
    cr print cr                     # Let's print the output!

    bar                          # <-- tree@ output

    cr :mytree @ treewalk        # walk the tree with treewalk

    key=foo, value=bar           # <-- treewalk output, not the line break via `cr`

    "vanilla" "favorite ice cream flavor" :mytree @ tree!
    :mytree @ treewalk           # walk the tree again; see new values
    key=foo, value=bar
    key=favorite ice cream flavor, value=vanilla
    "foo" :mytree @ treedel                        # delete a key
    :mytree @ treewalk
    key=favorite ice cream flavor, value=vanilla
    :mytree @ treedestroy                          # delete (destroy) the whole tree
    :mytree @ treewalk
                                                   # no output -- nothing to see
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
    dup strlen myfile @ fwrite            # write a sentence, specifying its length, to the file
    myfile @ fclose                       # close the file
    "test-file.txt" "r" fopen myfile !    # re-open for reading
    var buf 1024 mkbuf buf !              # create a memory buffer
    buf @ myfile @ 30 fread               # read 30 bytes from file, put in 'buf'
    # will print: Some text in my file! Woo-hoo!
    buf @ print cr
    myfile @ fclose                       # close the file
    ```

  * `tcplisten`, `tcpaccept` for server primitives, `tcpconnect` for clients. See the examples directory.


TODO:

  * More time functions (e.g. date, calendar stuff, etc.)
  * More string functions, as needed. We have most of the basic `string.h` C-level functions. What could be implemented
  would be things like search/replace and `regex`.
  * The `dsp.dc` lib has a good start! But it is growing still...this is an active
    area for my interests!
  * More OS-level integration (directories, permissions, etc.)
  * Turtle graphics for the kids!?


### Contact

Aaron Krister Johnson

Please report bugs and successes to <akjmicro@gmail.com>
