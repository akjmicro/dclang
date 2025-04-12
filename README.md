## dclang

### TO INSTALL AND SETUP:
____________

* Have the gcc (Linux) or clang (Mac) compiler on your machine.

* Install `portmidi` and the `portmidi.h` and `porttime.h` headers onto your system.

* Copy the appropriate `Makefile.[linux|mac]` based on your OS to `Makefile`:

  ```
  # Linux
  cp Makefile.linux Makefile

  # MacOS
  cp Makefile.mac Makefile
  ```

  ```
  # clone the repo and build
  git clone https://github.com/akjmicro/dclang
  cd dclang
  sudo make install

  # try running the primes example:
  dclang -i examples/some_primes.dc
  ```

* `make install` puts the executable `dclang` into `/usr/local/bin` by default.

* Experiment as you wish with compiler optimizations in the Makefile,
  particularly with float-point options, since 'dclang' is heavily
  reliant on them.

* You'll need to set `DCLANG_LIBS` to the location of your dclang source folder.
  ```
  export DCLANG_LIBS=/<this>/<source>/<folder>/<location>/lib
  ```

  If you use the `Makefile` directive `make install`; it will link
  the libs to `/usr/local/dclang/lib`, so you'd do:
  ```
  export DCLANG_LIBS=/usr/local/dclang/lib
  ```
  You can add this export statement to your shell (`bash`, `zsh`, etc.) startup script, of course.

* For interaction, it's nice to use 'rlwrap' to get readline line-history:
  ```
  rlwrap ./dclang
  ```
  One can also create an alias to `dclang` that uses `rlwrap`:
  ```
  alias dclang='rlwrap dclang`
  ```

* Interested in using an RPN language for MIDI coding? For MIDI instructions,
  check out the details in `examples/midi/README.md`

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

The user is encouraged to preuse the `examples/` and `tests/` directory to 
get a feel for the style and syntax of `dclang`. When you first launch an
interactive commandline session by typing `dclang` (or `rlwrap dclang` to
enable comand history, if you have `rlwrap`), you will see a categorized
listing of all the primitives. You can always see the list again by
calling the `primitives` word, which will show the info listing again.

A non-exhaustive notes on some of the things implemented thus far:

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
      from `ord` and return `-1` (true) or `0` (false) for determining the class of a given character.
      (N.B.: If given a string of len > 1, `ord` uses the first character of the string by default.)
    * regex primitives: `regcomp`, `regexec`, and `regread`
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
  * Linked lists: `lmake`, `lpush`, `lpop`, `l!`, `l@`, `lins`, `lrem`, `lsize`, `ldel`
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
    buf @ 30 myfile @ fread               # read 30 bytes from file, put in 'buf'
    .                                     # print num bytes read
    # will print: Some text in my file! Woo-hoo!
    buf @ print cr
    myfile @ fclose                       # close the file
    ```

  * `tcplisten`, `tcpaccept` for server primitives, `tcpconnect` for clients. See the examples directory.


### A note on "composibility"
After wrestling a bit back-n-forth with the stack API for trees and lists, I opted for what
what some might consider a non-standard approach to stack order for both. On the face of it,
the approach for these structures is inconsistent with the other variable types, "normal array slots"
and "hashes". Consider:

```
var thing
5 thing !         # normal 'FORTHish setter': <value> <slot> <action>
  thing @         # normal 'FORTHish getter':         <slot> <action>

# and for hashes:

"bar" "foo" h!
      "foo" h@
```

So, why have I not preserved this "standard stack-order way" for trees and lists? Well for starters, I did.
However, since these structures can sometimes be nested, and one can do neat things with nesting, keeping
this strictly FORTH-like way can be a nuisance, and unwieldy. Take for instance, a nested list:

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

Now, isn't the 2nd way much nicer, and easier to parse? For starters, we preserve the "odometer" feeling
of digging into a nested structure that most folks are used to. Also, notice how, related to that,
the idea of "outer" and "inner" is actually preserved in a left-to-right reading. Finally, when you
de-reference the `outer_list` with `l@`, you get the inner list, and can further deference _that_ in
a "chaining" fashion with yet another index and `l@` operator. I find this much clearer, cleaner, and
more intuitive.

Since trees can be nested in a similar way, I changed their stack-order expectations in a similar way.
There may come a time where I consider doing this to normal variables and hashes -- however, I've written
so much code, and a chain of dereferences making for strange stack-order (i.e. nesting) is less common
in those cases, and might be more upsetting the apple cart at this point. More thought and consideration
of the matter is required at this point to convince me that changing that is worth it.

### Contact

Aaron Krister Johnson

Please report bugs and successes to <akjmicro@gmail.com>
