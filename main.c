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
// end of data type macros

// input buffer and input file (stdin or file input) stuff
FILE *ifp;
char buf[IBUFSIZE];
MYINT bufused;
MYINT live_repl = 0;
// data stack
MYFLT data_stack[DATA_STACK_SIZE];
MYINT data_stack_ptr;
// return stack
MYINT return_stack[RETURN_STACK_SIZE];
MYINT return_stack_ptr;
// loop 'stack'
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

// flag for if we are defining a new word (function)
MYINT def_mode;

// inline dclang code
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
#include "input_ops.c"


void show_primitivesfunc() {
    printf("\n");
    int prim_size = sizeof(primitives) / sizeof(primitives[0]);
    for (int x=0; x < (prim_size - 1); x++) {
        printf("%s ", primitives[x].name);
    }
    printf("\n");
}

// needed so we can add 'import' to primitives
int load_extra_primitives() {
    primitives[74].name = "import";
    primitives[74].function = importfunc;
    primitives[75].name = "show-primitives";
    primitives[75].function = show_primitivesfunc;
    primitives[76].name = "show-user-functions";
    primitives[76].function = showdefined;
    primitives[77].name = "repl";
    primitives[77].function = repl;
    primitives[78].name = 0;
    primitives[78].function = 0;
    return 1;
}

// Where all the juicy fun begins... 
int main(int argc, char **argv)
{
    setinput(stdin);
    load_extra_primitives();
    srand(time(NULL));
    //setlocale(LC_ALL, "");
    if (argc > 1) {
        for(int opt = 1; opt < argc; opt++) {
            if (strcmp(argv[opt], "-i") == 0) {
                live_repl = 1;
            } else {
                import(argv[opt]);
            }   
        };
    } else {
        live_repl = 1;
    }
    if (live_repl) {
        printf("Welcome to dclang! Aaron Krister Johnson, 2019\n");
        printf("Make sure to peruse README.md to get your bearings!\n");
        repl();
    }
}
