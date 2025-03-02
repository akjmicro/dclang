/*
   These should be changed based on architecture. For instance, on my x86_64
   system, best performance was squeezed by making the integer type and the
   float type to both be optimized in alignment to 8-bytes, which turns out
   to be 'long' for integers, and 'double' for floating-point values.
   On the Raspberry Pi 3, probably best to use 'long' and 'double' as well.
   I've found it crawls to a halt on the benchmarks if you use 'float'!
*/

#define _GNU_SOURCE
#define __USE_GNU 1

#include <ctype.h>
#include <stdio.h>
#include "noheap/malloc.c"
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <regex.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <inttypes.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <time.h>
#include <portmidi.h>
#include <porttime.h>
#include <sqlite3.h>

// data type macros
#define DCLANG_FLT   double
#define DCLANG_LONG  long
#define DCLANG_INT   int
#define DCLANG_PTR   uintptr_t
#define DCLANG_ULONG unsigned long
#define DCLANG_UINT  unsigned int
// end of data type macros

#define DATA_STACK_SIZE   32
#define RETURN_STACK_SIZE 128
#define MAXWORD           1048576
#define MAX_OPCODES       208
#define IBUFSIZE          128
#define NUMLOCALS         8
#define NUMVARS           16777216
#define NUM_TREE_ROOTS    32

// compiled tokens get saved and put into an array of type 'inst_struct'
typedef struct {
    int opcode;
    DCLANG_FLT param;
} inst_struct;

// an array of inst_struct instructions. This is where the user's commands,
// i.e. the 'program' will live:
inst_struct  prog[MAXWORD];
DCLANG_PTR   iptr;
DCLANG_PTR   max_iptr;

// data stack
DCLANG_FLT   data_stack[DATA_STACK_SIZE * 2];
DCLANG_PTR   data_stack_ptr;
DCLANG_PTR   save_data_stack_ptr;
// return stack
DCLANG_LONG  return_stack[RETURN_STACK_SIZE];
DCLANG_PTR   return_stack_ptr;
// local variables
DCLANG_PTR   locals_keys[NUMLOCALS];
DCLANG_FLT   locals_vals[NUMLOCALS * RETURN_STACK_SIZE];
DCLANG_PTR   locals_base_idx;
// global variables
// track variables - mapping from name to index
char        *var_keys[NUMVARS];
DCLANG_PTR   var_vals[NUMVARS];
DCLANG_PTR   var_map_idx;
// variable actual data stored here
DCLANG_FLT   vars[NUMVARS];
DCLANG_PTR   vars_idx;
// global constants
// track constants - mapping from name to index
char        *const_keys[64];
DCLANG_FLT   const_vals[64];
DCLANG_PTR   const_idx;

// hashwords
char **hashwords;
DCLANG_ULONG hashwords_size = 32;
DCLANG_ULONG hashwords_cnt = 0;
// min and max string buffer addresses
DCLANG_PTR MIN_STR = 0;
DCLANG_PTR MAX_STR = 0;

// we need to populate the dictionary first with primitives
struct primitive {
  const char *name;
  char *category;
  int opcode;
} primitive __attribute__((aligned(16)));

struct primitive *primitives;
int primitives_idx = -1;

// The dispatch table for computed gotos
static void *dispatch_table[MAX_OPCODES];

// opcode enumeration
enum dclang_opcodes {
    OP_NOOP,
    OP_PUSH,
    OP_PUSH_NO_CHECK,
    OP_DROP,
    OP_DUP,
    OP_OVER,
    OP_PICK,
    OP_SWAP,
    OP_DROP2,
    OP_DUP2,
    OP_OVER2,
    OP_DEPTH,
    OP_CLEAR,
    OP_SVPUSH,
    OP_SVPOP,
    OP_SVDROP,
    OP_SVPICK,
    OP_SVDEPTH,
    OP_SVCLEAR,
    OP_SHOW,
    OP_SHOWNOSPACE,
    OP_SHOWRJ,
    OP_SHOWPZ,
    OP_SHOWSTACK
};

    /*
    // booleans
    OP_NULL,
    OP_FALSE,
    OP_TRUE,
    OP_EQ,
    OP_NOTEQ,
    OP_LT,
    OP_GT,
    OP_LTE,
    OP_GTE,
    OP_ASSERT,
    // bit manipulation
    OP_AND,
    OP_OR,
    OP_XOR,
    OP_NOT,
    OP_LSHIFT,
    OP_RSHIFT,
    // basic arithmetic
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_ABS,
    OP_MIN,
    OP_MAX,
    // float -> int
    OP_ROUND,
    OP_CEIL,
    OP_FLOOR,
    // higher math
    OP_POWER,
    OP_SQRT,
    OP_EXP,
    OP_LOG,
    OP_LOG2,
    OP_LOG10,
    OP_E,
    // trig
    OP_PI,
    OP_SINE,
    OP_COS,
    OP_TAN,
    OP_ASINE,
    OP_ACOS,
    OP_ATAN,
    OP_ATAN2,
    OP_ATAN,
    OP_ATAN,
    OP_ATAN,
    // randomness
    OP_RAND,
    // variables and arrays
    OP_VARIABLE,
    OP_POKE,
    OP_PEEK,
    OP_ALLOT,
    OP_CREATE,
    OP_COMMA,
    OP_HERE,
    OP_CONSTANT,
    OP_ENVGET,
    OP_ENVSET,
    // sorting
    OP_SORTNUMS,
    OP_SORTSTRS,
    // hash set & get
    OP_HASHSET,
    OP_HASHGET,
    OP_HASHKEYS,
    // tree storage
    OP_TREEMAKE,
    OP_TREESET,
    OP_TREEGET,
    OP_TREEWALK,
    OP_TREEDELETE,
#ifdef HAS_TREEDESTROY,
    OP_TREEDESTROY,
#endif,
    // linked lists
    OP_LISTMAKE,
    OP_LISTNEXT,
    OP_LISTSET,
    OP_LISTGET,
    OP_LISTPUSH,
    OP_LISTPOP,
    OP_LISTINSERT,
    OP_LISTREMOVE,
    OP_LISTSIZE,
    OP_LISTDELETE,
    // branching/control
    OP_TIMES,
    OP_AGAIN,
    OP_EXITTIMES,
    OP_FOR,
    OP_NEXT,
    OP_EXITFOR,
    OP_I,
    OP_J,
    OP_K,
    OP_IF,
    OP_ELSE,
    OP_ENDIF,
    OP_RETURN,
    // character emitters
    OP_EMIT,
    OP_UEMIT,
    OP_BYTES32,
    // character types
    OP_ISALNUM,
    OP_ISALPHA,
    OP_ISCNTRL,
    OP_ISDIGIT,
    OP_ISGRAPH,
    OP_ISLOWER,
    OP_ISPRINT,
    OP_ISPUNCT,
    OP_ISSPACE,
    OP_ISUPPER,
    OP_ISXDIGIT,
    // output and string ops
    OP_CR,
    OP_PRINT,
    // string conversion
    OP_TOHEX,
    OP_TOSTR,
    OP_TONUM,
    OP_ORD,
    OP_TOLOWER,
    OP_TOUPPER,
    // general string ops from C
    OP_STRLEN,
    OP_STREQ,
    OP_STRLT,
    OP_STRGT,
    OP_STRFIND,
    OP_STRSPN,
    OP_STRCSPN,
    OP_STRTOK,
    OP_MEMPCPY,
    OP_MEMSET,
    // memory buffers
    OP_MKBUF,
    OP_FREE,
    OP_MEMUSED,
    // regex
    OP_REGCOMP,
    OP_REGEXEC,
    OP_REGREAD,
    // file
    OP_FILEOPEN,
    OP_FILEMEMOPEN,
    OP_FILEREAD,
    OP_FILEREADLINE,
    OP_FILEREADALL,
    OP_FILESEEK,
    OP_FILETELL,
    OP_FILEWRITE,
    OP_FILEFLUSH,
    OP_FILECLOSE,
    OP_REDIRECT,
    OP_RESETOUT,
    OP_FLUSHOUT,
    // low-level (OS) file ops:
    OP_OPEN,
    OP_READ,
    OP_WRITE,
    OP_CLOSE,
    // SQLite3 interface
    OP_SQLITEOPEN,
    OP_SQLITEPREPARE,
    OP_SQLITESTEP,
    OP_SQLITECOLUMN,
    OP_SQLITEFINALIZE,
    OP_SQLITEEXEC,
    OP_SQLITECLOSE,
    // tcp/udp networking using sockets
    OP_TCPLISTEN,
    OP_TCPACCEPT,
    OP_TCPCONNECT,
    OP_UDPRECV,
    OP_UDPSEND,
    // time
    OP_CLOCK,
    OP_SLEEP,
    OP_EPOCH,
    OP_EPOCH,
    OP_DT,
    // block a SIGINT
    OP_BLOCKSIGINT,
    OP_UNBLOCKSIGINT,
    // portmidi
    OP_PM_LIST,
    OP_PM_OPENOUT,
    OP_PM_WS,
    OP_PM_WSR,
    OP_PM_CLOSE,
    OP_PM_TERMINATE,
    // os fork and exit
    OP_FORK,
    OP_EXIT,
    // show defined words!
    OP_SHOWDEFINED,
    OP_SHOWCONSTS,
    OP_SHOWVARS
    */

// user words
typedef struct {
    const char *name;
    DCLANG_PTR word_start;
} user_word;

user_word   user_words[1024];
DCLANG_PTR  num_user_words;

// flag for if we are defining a new word (function)
DCLANG_LONG def_mode;
// input buffer and input file (stdin or file input) stuff
FILE *ifp;
char buf[IBUFSIZE];
DCLANG_LONG bufused;
DCLANG_LONG live_repl = 0;
// output file, usually stdout, but can be redirected with 'redirect'
FILE *ofp;
// file stack for imports
FILE *file_stack[32];
DCLANG_PTR fsp;

struct timeval tval;

///////////////
// API calls //
///////////////

extern void dclang_initialize();

extern DCLANG_LONG dclang_import(char *infilestr);

extern DCLANG_LONG dclang_findword(const char *word);

extern void dclang_callword(DCLANG_PTR where);

extern DCLANG_FLT dclang_pop();
