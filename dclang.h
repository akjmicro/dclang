/*
   These should be changed based on architecture. For instance, on my x86_64
   system, best performance was squeezed by making the integer type and the
   float type to both be optimized in alignment to 8-bytes, which turns out
   to be 'long' for integers, and 'double' for floating-point values.
   On the Raspberry Pi 3, probably best to use 'long' and 'double' as well.
   I've found it crawls to a halt on the benchmarks if you use 'float'!
*/

#include <inttypes.h>

// data type macros
#define DCLANG_FLT     double
#define DCLANG_INT     long
#define DCLANG_INT32   int
#define DCLANG_PTR     uintptr_t
#define DCLANG_UINT    unsigned long
#define DCLANG_UINT32  unsigned int
// end of data type macros

#define DATA_STACK_SIZE 128
#define RETURN_STACK_SIZE 128
#define MAXWORD 1048576
#define IBUFSIZE 128
#define NUMVARS 16777216
#define NUM_TREE_ROOTS 32

// compiled tokens get saved and put into an array of type 'inst_struct'
typedef union {
    void (*with_param) (DCLANG_FLT);
    void (*without_param) (void);
} func_union;

typedef struct {
    func_union function;
    DCLANG_FLT param;
} inst_struct;

// an array of inst_struct instructions. This is where the user's commands,
// i.e. the 'program' will live:
inst_struct  prog[MAXWORD];
DCLANG_PTR   iptr;
DCLANG_PTR   max_iptr;

// data stack
DCLANG_FLT data_stack[DATA_STACK_SIZE * 2];
DCLANG_PTR data_stack_ptr;
// return stack
DCLANG_INT return_stack[RETURN_STACK_SIZE];
DCLANG_PTR return_stack_ptr;
// loop 'stack'
DCLANG_INT loop_counter[3];
DCLANG_PTR loop_counter_ptr;

// save data stack
DCLANG_FLT save_data_stack[DATA_STACK_SIZE];
DCLANG_PTR save_data_stack_ptr;
// variables space
DCLANG_FLT vars[NUMVARS];
DCLANG_PTR varsidx;
// hashwords
char **hashwords;
DCLANG_UINT hashwords_size = 32;
DCLANG_UINT hashwords_cnt = 0;
// min and max string buffer addresses
DCLANG_PTR MIN_STR = 0;
DCLANG_PTR MAX_STR = 0;

// we need to populate the dictionary first with primitives
struct primitive {
  const char *name;
  void (*function) (void);
} primitive;

struct primitive *primitives;
int primitives_idx = -1;

// user words
typedef struct {
    const char *name;
    DCLANG_PTR word_start;
} user_word;

user_word   user_words[1024];
DCLANG_PTR  num_user_words;

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
DCLANG_PTR fsp;

///////////////
// API calls //
///////////////

extern void dclang_initialize();

extern DCLANG_INT dclang_import(char *infilestr);

extern DCLANG_INT dclang_findword(const char *word);

extern void dclang_callword(DCLANG_PTR where);

extern DCLANG_FLT dclang_pop();
