/* dclang, an RPN programming language.

'True refinement seeks simplicity.' - Bruce Lee

Born on 2018-05-05 */

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

#define DATA_STACK_SIZE 128
#define RETURN_STACK_SIZE 128
#define DELIM " "
#define MAXWORD 65536
#define IBUFSIZE 128

/*
   These should be changed based on architecture. For instance, on my x86_64
   system, best performance was squeezed by making the integer type and the
   float type to both be optimized in alignment to 8-bytes, which turns out
   to be 'long' for integers, and 'double' for floating-point values.
   On the Raspberry Pi 3, probably best to use 'long' and 'double' as well.
   I've found it crawls to a halt on the benchmarks if you use 'float'!
*/

#define MYINT32 int
#define MYUINT32 unsigned int
#define MYINT long
#define MYUINT unsigned long
#define MYFLT double
// end of data type macros

// input buffer and input file (stdin or file input) stuff
FILE *ifp;
char buf[IBUFSIZE];
MYINT bufused;
MYINT live_repl = 0;
// output file, usually stdout, but can be redirected with 'redirect'
FILE *ofp;
// data stack
MYFLT data_stack[DATA_STACK_SIZE];
int data_stack_ptr;
MYFLT save_data_stack[DATA_STACK_SIZE];
int save_data_stack_ptr;
// return stack
MYINT return_stack[RETURN_STACK_SIZE];
int return_stack_ptr;
// loop 'stack'
MYINT loop_counter[3];
int loop_counter_ptr;
// file stack for imports
FILE *file_stack[32];
MYINT fsp;

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
MYUINT max_iptr;

// flag for if we are defining a new word (function)
MYINT def_mode;

// inline dclang code
#include "stack_ops.c"
#include "null_ops.c"
#include "logic_ops.c"
#include "time_ops.c"
#include "math_ops.c"
#include "branch_ops.c"
#include "output_ops.c"
#include "string_ops.c"
#include "token.c"
#include "file_ops.c"
#include "user_words.c"
#include "variable_ops.c"
#include "operators.c"
#include "input_ops.c"


// needed so we can add 'import' to primitives
void load_extra_primitives() {
    primitives[121].name = "primitives";
    primitives[121].function = show_primitivesfunc;
    primitives[122].name = "import";
    primitives[122].function = importfunc;
    primitives[123].name = "input";
    primitives[123].function = inputfunc;
    /* final endpoint must be zeros,
       and they won't count in the 'count': */
    primitives[124].name = 0;
    primitives[124].function = 0;
}


// Where all the juicy fun begins...
int main(int argc, char **argv)
{
    setinput(stdin);
    resetoutfunc();
    load_extra_primitives();
    srand(time(NULL));
    // create the global hash table
    hcreate(1048576);
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
