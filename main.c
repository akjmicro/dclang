/* dclang, an RPN calculator that has dreams of being a full-blown
Turing-complete programming language, a descendant of FORTH in minimalism
and philosophy.  Born on 2018-05-05 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#define DATA_STACK_SIZE 128
#define RETURN_STACK_SIZE 256
#define DELIM " "
#define MAXWORD 65536
#define IBUFSIZE 128

/* These should be changed based on architecture. For instance, on my x86_64
   system, best performance was squeezed by making the integer type and the
   float type to both be optimized in alignment to 8-bytes, which turns out
   to be 'long' for integers, and 'double' for floating-point values. */
#define MYINT long
#define MYFLT double
/* end of data type macros */

char buf[IBUFSIZE];
MYINT bufused;
/* data stack */
MYFLT data_stack[DATA_STACK_SIZE];
MYINT data_stack_ptr;
/* return stack */
MYINT return_stack[RETURN_STACK_SIZE];
MYINT return_stack_ptr;
/* loop 'stack' */
MYINT loop_counter[3];
MYINT loop_counter_ptr;

// compiled tokens get saved and put into an array of type 'inst_struct'
typedef union {
    void (*with_param) (MYFLT);
    void (*without_param) (void);
} func_union;
typedef struct {
    func_union function;
    MYFLT param;
} inst_struct;

/* an array of inst_struct instructions. This is where the user's commands,
 i.e. the 'program' will live: */
inst_struct prog[MAXWORD];
MYINT iptr;

/* flag for if we are defining a new word (function) */
MYINT def_mode;

/* inline dclang code */
#include "stack_ops.c"
#include "logic_ops.c"
#include "time_ops.c"
#include "math_ops.c"
#include "branch_ops.c"
#include "output_ops.c"
#include "string_ops.c"
#include "variable_ops.c"
#include "token.c"
#include "user_functions.c"
#include "operators.c"

const char *illegal[] = {"do", "redo", "exitdo",
                         "qdo", "qredo",
                         "for", "next", "exitfor",
                         "skip"};
MYINT num_illegal = sizeof(illegal) / sizeof(illegal[0]); 

/* function to validate and return an error message if we are using control
 * structures outside of a definition */
static MYINT validate(const char *token)
{
    MYINT checkval = 1;
    for (MYINT i=0; i < num_illegal; i++) {
        if(strcmp(token, illegal[i]) == 0) {
            printf("Error: '%s' -- illegal outside of function def.\n",
                   illegal[i]);
            checkval = 0;
            return checkval;
        }
    }
    return checkval;
}

/* function to compile each incoming token */
static void compile_or_interpret(const char *argument)
{
    char *endPointer = 0;
    double d;
    const struct primitive *pr = primitives;

    if (argument == 0) {
        return;
    } 

    /* search dictionary (list, not hash) entry */
    while (pr->name != 0) {
        if (strcmp(pr->name, argument) == 0) {
            if (def_mode) {
                /* insert function only if it's not 's"' for strings: */
                if (strcmp("s\"", argument) != 0) {
                    prog[iptr++].function.without_param = pr->function;
                } else {
                    (*(pr->function)) ();
                }
            } else {
                if (validate(argument)) {
                    (*(pr->function)) ();
                }
            }
            return;
        }
        pr++;
    }

    /* search user-defined functions (words) */
    for (MYINT x = num_user_functions - 1; x > -1 ; x--) {
        if (strcmp(user_functions[x].name, argument) == 0) {
            if (def_mode) {
                prog[iptr].function.with_param = gotofunc;
                prog[iptr++].param = user_functions[x].func_start;
            } else {
                MYINT cur_iptr = iptr;
                gotofunc(user_functions[x].func_start);
                /* run the function */
                while (iptr < cur_iptr) {
                    iptr += 1;
                    (*(prog[iptr].function.with_param)) (prog[iptr].param);                    
                }
            }
            return;
        }
    }

    /* primitive not found, user definitions not found.  OK, so next, try to
    convert to a number */
    d = strtod(argument, &endPointer);
    if (endPointer != argument) {
        if (def_mode) {
            prog[iptr].function.with_param = push;
            prog[iptr++].param = d;
        } else {
            push(d);    
        }
        return;
    }
    
    /* nothing found, we've reached an error condition, so report
    the situation and reset the stack */
    data_stack_ptr = 0;
    printf("%s -- syntax error.\n", argument); 
    return;    
}


/* Where all the juicy fun begins... */
MYINT main(int argc, char **argv)
{
    srand(time(NULL));
    //setlocale(LC_ALL, "");
    printf("Welcome to dclang! Aaron Krister Johnson, 2018\n");
    printf("Make sure to peruse README.md to get your bearings!\n");
    while (1) {
        /* get next input token */
        char *token;
        token = get_token();
        /* are we dealing with a function definition? */
        if (strcmp(token, "[") == 0) {
            startdeffunc();
            def_mode = 1;
            continue; // goto top of loop
        }
        if (strcmp(token, "]") == 0) {
            enddeffunc();
            def_mode = 0;
            continue; // goto top of loop
        }
        /* 'compile' it, or interpret it on-the-fly */
        compile_or_interpret(token);
    }
    compile_or_interpret(0);
    return EXIT_SUCCESS;
}
