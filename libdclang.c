/* dclang, an RPN programming language.

'True refinement seeks simplicity.' - Bruce Lee

Born on 2018-05-05 */

#define _GNU_SOURCE
#define __USE_GNU 1

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <fcntl.h>
#include <search.h>
#include <signal.h>

/*
   These should be changed based on architecture. For instance, on my x86_64
   system, best performance was squeezed by making the integer type and the
   float type to both be optimized in alignment to 8-bytes, which turns out
   to be 'long' for integers, and 'double' for floating-point values.
   On the Raspberry Pi 3, probably best to use 'long' and 'double' as well.
   I've found it crawls to a halt on the benchmarks if you use 'float'!
*/

// data type macros
#define DCLANG_INT32 int
#define DCLANG_UINT32 unsigned int
#define DCLANG_INT long
#define DCLANG_UINT unsigned long
#define DCLANG_FLT double
// end of data type macros

#define DATA_STACK_SIZE 128
#define RETURN_STACK_SIZE 128
#define MAXWORD 1048576
#define IBUFSIZE 128
#define NUMVARS 16777216
#define NUM_TREE_ROOTS 32

// data stack
DCLANG_FLT data_stack[DATA_STACK_SIZE * 2];
DCLANG_UINT32 data_stack_ptr;
// save data stack
DCLANG_FLT save_data_stack[DATA_STACK_SIZE];
DCLANG_UINT32 save_data_stack_ptr;
// variables space
DCLANG_FLT vars[NUMVARS];
DCLANG_UINT32 varsidx;
// return stack
DCLANG_INT return_stack[RETURN_STACK_SIZE];
DCLANG_UINT32 return_stack_ptr;
// loop 'stack'
DCLANG_INT loop_counter[3];
DCLANG_UINT32 loop_counter_ptr;
// min and max string buffer addresses
DCLANG_UINT MIN_STR = 0;
DCLANG_UINT MAX_STR = 0;

// compiled tokens get saved and put into an array of type 'inst_struct'
typedef union {
    void (*with_param) (DCLANG_FLT);
    void (*without_param) (void);
} func_union;
typedef struct {
    func_union function;
    DCLANG_FLT param;
} inst_struct;

/* an array of inst_struct instructions. This is where the user's commands,
 i.e. the 'program' will live: */
inst_struct prog[MAXWORD];
DCLANG_INT iptr;
DCLANG_UINT max_iptr;

// flag for if we are defining a new word (function)
DCLANG_INT def_mode;

// input buffer and input file (stdin or file input) stuff
FILE *ifp;
char buf[IBUFSIZE];
DCLANG_INT bufused;
DCLANG_INT live_repl = 0;
// output file, usually stdout, but can be redirected with 'redirect'
FILE *ofp;
// file stack for imports
FILE *file_stack[32];
DCLANG_INT fsp;

// inline dclang code
#include "stack_ops.c"
#include "logic_ops.c"
#include "math_ops.c"
#include "branch_ops.c"
#include "output_ops.c"
#include "time_ops.c"
#include "file_ops.c"
#include "socket_ops.c"
#include "string_ops.c"
#include "char_ops.c"
#include "token.c"
#include "user_words.c"
#include "variable_ops.c"
#include "tree_ops.c"
#include "signal_ops.c"
#include "primitives.c"
#include "input_ops.c"

void execfunc() {
    if (data_stack_ptr < 1)
    {
        printf("exec -- stack underflow! ");
        return;
    }
    DCLANG_UINT string_uint_addr = (DCLANG_UINT) dclang_pop();
    if (string_uint_addr < MIN_STR || string_uint_addr > MAX_STR)
    {
        perror("exec -- String address out-of-range.");
        return;
    }
    char *argument = (char *)string_uint_addr;
    const struct primitive *pr = primitives;

    // search user-defined functions (words)
    DCLANG_INT found = dclang_findword(argument);
    if (found > -1) {
        if (def_mode) {
            prog[iptr].function.with_param = callword;
            prog[iptr++].param = found;
        } else {
            dclang_callword(found);
        }
        return;
    }

    // search dictionary (list, not hash) entry
    while (pr->name != 0) {
        if (strcmp(pr->name, argument) == 0) {
            if ((def_mode) && (!is_special_form(pr->name))) {
                prog[iptr++].function.without_param = pr->function;
            } else {
                if (validate(argument)) {
                    (*(pr->function)) ();
                }
            }
            return;
        }
        pr++;
    }
    printf("exec -- word not found: %s\n", argument);
}

// needed so we can add 'import' to primitives
void load_extra_primitives()
{
    add_primitive("primitives", show_primitivesfunc);
    add_primitive("import", importfunc);
    add_primitive("input", inputfunc);
    add_primitive("exec", execfunc);
    /* final endpoint must be zeros,
       and they won't count in the 'count': */
    add_primitive(0, 0);
}

void dclang_initialize()
{
    setinput(stdin);
    resetoutfunc();
    add_all_primitives();
    load_extra_primitives();
    srand(time(NULL));
    // create the global hash table
    hcreate(1048576);
    hashwords = (char**)malloc(hashwords_size * sizeof(*hashwords));
}
