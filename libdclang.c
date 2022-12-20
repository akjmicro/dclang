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

// our main types, etc.
#include "dclang.h"

// inline dclang code
#include "stack_ops.c"
#include "logic_ops.c"
#include "math_ops.c"
#include "output_ops.c"
#include "file_ops.c"
#include "socket_ops.c"
#include "signal_ops.c"
#include "time_ops.c"
#include "string_ops.c"
#include "char_ops.c"
#include "token.c"
#include "branch_ops.c"
#include "user_words.c"
#include "variable_ops.c"
#include "tree_ops.c"
#include "primitives.c"
#include "input_ops.c"

void execfunc() {
    if (data_stack_ptr < 1)
    {
        printf("exec -- stack underflow! ");
        return;
    }
    DCLANG_PTR string_uint_addr = (DCLANG_PTR) dclang_pop();
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
