## dclang

### TO INSTALL:
____________

* Have the gcc compiler on your machine

```
    git clone https://github.com/akjmicro/dclang
    cd dclang
    make
    ./dclang
```

* You can also put the executable in /usr/local/bin or what-have-you.

* Experiment as you wish with compiler optimizations in the Makefile,
  particularly with floating-point options, since 'dclang' is heavily
  reliant on them.

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

    begin i 7 < if i else exit endif again
    .s
    <7> 0 1 2 3 4 5 6 

```

Notice the '.' character, which pops/prints the top-of-stack (TOS). This comes
from FORTH, as does '.s', which non-destructively shows the stack contents.
This is different from 'dc', where 'p' pops/prints the TOS.

In the looping example, the 'begin/again' block has access to a hiddne
variable 'i', which you can use to test conditionally and escape the loop.
This is a weird mixture of the way FORTH uses 'do/loop' and 'begin/again';
note how unlike traditional FORTH, you manually test the condition inside
the block yourself. I may change this in the future to be more FORTH-like,
and have separate 'do/loop' and 'begin/again' constructs.

This project is *far* from complete, but the goal is for it to be a full-blown
Turing-complete language in the vein of FORTH.

So far, I've implemented:

  * +, -, *, /, %, abs
  * pow, sqrt, log, log2
  * sin, cos, tan
  * and, or, not, xor
  * =, <>, >, <, >=, <= 
  * if-else-endif
  * begin-again (looping)
  * drop, dup, swap, over, rot, 2drop, 2dup, 2swap, 2over, 2rot
  * a clock function so we can time execution

TODO:

  * open/read/write/close to the filesystem
  * more time functions (e.g. date, sleep, etc.)
  * strings and basic functionality around them
  * turtle graphics for the kids!
  * just about everything a usuable language will need, or at least, the
  means for someone to hook C-libraries into this enchilada.
  * although it's beating or matching gforth at things like immediate
  floating-point operations, it's pretty slow at looping.  I need to address
  that at some point.

Everything is on the floating-point stack only at this point.  I will
obviously have to change/add much in the way of the structure to implement
other things.  This is very much ALPHA right now, but it's working great and
very fun so far.

### contact

Aaron Krister Johnson

Please report bugs and successes to aaron@untwelve.org
