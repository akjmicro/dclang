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

#define DATA_STACK_SIZE 256
#define RETURN_STACK_SIZE 256
#define DELIM " "
#define MAXWORD 65536
#define IBUFSIZE 128

// This should be changed based on architecture. Experiment between 'int' and 'long'
#define MYINT int
// input buffer and input file (stdin or file input) stuff
FILE *ifp;
char buf[IBUFSIZE];
MYINT bufused;
MYINT live_repl = 0;
// output file, usually stdout, but can be redirected with 'redirect'
FILE *ofp;
// data stack
MYINT data_stack[DATA_STACK_SIZE];
register int data_stack_ptr asm("r7");
// return stack
MYINT return_stack[RETURN_STACK_SIZE];
register int return_stack_ptr asm("r8");
// loop 'stack'
MYINT loop_counter[3];
register int loop_counter_ptr asm("r9");
// file stack for imports
FILE *file_stack[32];
MYINT fsp;

// compiled tokens get saved and put into an array of type 'inst_struct'
typedef union {
    void (*with_param) (MYINT);
    void (*without_param) (void);
} func_union;
typedef struct {
    func_union function;
    MYINT param;
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
#include "file_ops.c"
#include "user_functions.c"
#include "operators.c"
#include "input_ops.c"


// needed so we can add 'import' to primitives
void load_extra_primitives() {
    primitives[74].name = "show-primitives";
    primitives[74].function = show_primitivesfunc;
    primitives[75].name = "import";
    primitives[75].function = importfunc;
    // final endpoint must be zeros,
    // and they won't count in the 'count':
    primitives[76].name = 0;
    primitives[76].function = 0;
}

// Where all the juicy fun begins... 
int main(int argc, char **argv)
{
    setinput(stdin);
    resetoutfunc();
    //load_extra_primitives();
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
        show_primitivesfunc();
        repl();
    }
}
