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

#define DELIM " "
#define MAXWORD 65536

#define IBUFSIZE 128
char buf[IBUFSIZE];
int bufused;
long double data_stack[128];
register long int data_stack_ptr asm("r15");
long int return_stack[256];
register long int return_stack_ptr asm("r14");
long int loop_counter[3];
long int loop_counter_ptr;

// compiled tokens get saved and put into an array of type 'prog_struct'
typedef union {
    void (*with_param) (long double);
    void (*without_param) (void);
} func_union;
typedef struct {
    func_union function;
    long double param;
} inst_struct;

/* an array of inst_struct instructions. This is where the user's commands,
 i.e. the 'program' will live: */
inst_struct prog[MAXWORD];
long int iptr;

/* flag for if we are defining a new word (function) */
int def_mode;

/* inline dclang code */
#include "stack_ops.c"
#include "logic_ops.c"
#include "math_ops.c"
#include "branch_ops.c"
#include "time_ops.c"
#include "output_ops.c"
#include "string_ops.c"
#include "variable_ops.c"
#include "token.c"
#include "user_functions.c"
#include "operators.c"

/* function to validate and return an error message if we are using control
 * structures outside of a definition */
static int validate(const char *token)
{
    int checkval = 1;
    if (strcmp("do", token) == 0) {
        printf("Error: 'do' illegal use outside of function definition\n");
        return 0;
    }
    if (strcmp("redo", token) == 0) {
        printf("Error: 'redo' illegal use outside of function definition\n");
        return 0;
    }
    if (strcmp("exit", token) == 0) {
        printf("Error: 'exit' illegal use outside of function definition\n");
        return 0;
    }
    if (strcmp("skip", token) == 0) {
        printf("Error: 'skip' illegal use outside of function definition\n");
        return 0;
    }
    return checkval;
}

/* function to compile each incoming token */
static void compile_or_interpret(const char *argument)
{
    char *endPointer = 0;
    long double d;
    const struct primitive *pr = primitives;

    if (argument == 0) {
        //printfunc();
        return;
    } 

    /* search dictionary (list, not hash) entry */
    while (pr->name != 0) {
        if (strcmp(pr->name, argument) == 0) {
            if (def_mode) {
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

    /* next, search for user-defined procedures.  If found, insert into the
    'prog' array a built-in function that takes the current 'iptr' location,
    pushes it onto the return stack, and then takes the function location and
    jumps to it when it is executed. The pre-defined function, on the other
    end, have a 'return' function that will pop (restore) the iptr location
    off the return stack, and so it will be what it was before the jump.
    
    Where is the function definition? Inside the 'prog' array, of course! The
    only thing different about it is that its start location will be noted and
    saved in a special struct array, similar to the way primitives are looked
    up, and it will have a 'return' automatically inserted on its tail. */
    for (int x = 0; x < num_user_functions; x++) {
        if (strcmp(user_functions[x].name, argument) == 0) {
            if (def_mode) {
                //printf("matching %s in function def\n", argument);
                prog[iptr].function.with_param = gotofunc;
                prog[iptr++].param = user_functions[x].func_start;
            } else {
                long int cur_iptr = iptr;
                gotofunc(user_functions[x].func_start);
                /* run the function */
                while (iptr < cur_iptr) {
                    iptr += 1;
                    //printf("Executing instruction %i\n", iptr);               
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
int main(int argc, char **argv)
{ 
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
