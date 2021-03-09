/* dclang, an RPN programming language.

'True refinement seeks simplicity.' - Bruce Lee

Born on 2018-05-05 */

#define _GNU_SOURCE

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
#define NUMVARS 1048576

// data stack
DCLANG_FLT data_stack[DATA_STACK_SIZE * 2];
int data_stack_ptr;
// save data stack
DCLANG_FLT save_data_stack[DATA_STACK_SIZE];
int save_data_stack_ptr;
// variables space
DCLANG_FLT vars[NUMVARS];
DCLANG_UINT varsidx;
// return stack
DCLANG_INT return_stack[RETURN_STACK_SIZE];
int return_stack_ptr;
// loop 'stack'
DCLANG_INT loop_counter[3];
int loop_counter_ptr;
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
#include "null_ops.c"
#include "logic_ops.c"
#include "math_ops.c"
#include "branch_ops.c"
#include "output_ops.c"
#include "time_ops.c"
#include "file_ops.c"
#include "socket_ops.c"
#include "string_ops.c"
#include "token.c"
#include "user_words.c"
#include "variable_ops.c"
#include "primitives.c"
#include "input_ops.c"


// needed so we can add 'import' to primitives
void load_extra_primitives() {
    add_primitive("primitives", show_primitivesfunc);
    add_primitive("import", importfunc);
    add_primitive("input", inputfunc);
    /* final endpoint must be zeros,
       and they won't count in the 'count': */
    add_primitive(0, 0);
}


// Where all the juicy fun begins...
int main(int argc, char **argv)
{
    setinput(stdin);
    resetoutfunc();
    add_all_primitives();
    load_extra_primitives();
    srand(time(NULL));
    // create the global hash table
    hcreate(1048576);
    hashwords = (char**)malloc(hashwords_size * sizeof(*hashwords));
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
        printf("Welcome to dclang! Aaron Krister Johnson, 2020\n");
        printf("Make sure to peruse README.md to get your bearings!\n");
        printf("You can type 'primitives' to see a list of all the primitive (c-builtin) words.\n");
        printf("You can type 'words' to see a list of functions defined within dclang.\n");
        show_primitivesfunc();
        repl();
    }
}
