/* dclang, an RPN programming language.

'True refinement seeks simplicity.' - Bruce Lee

Born on 2018-05-05 */

#define _GNU_SOURCE
#define __USE_GNU 1

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <regex.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <inttypes.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <time.h>
#include <portmidi.h>
#include <porttime.h>
#include <sqlite3.h>
#include "./noheap/malloc.c"
#include "./noheap/ht.c"
#include "./noheap/trees.c"
#include "./noheap/llist.c"

// data type macros
#if UINTPTR_MAX == UINT64_MAX
#define DCLANG_FLT   double
typedef __int128 DCLANG_WIDE_INT;
#elif UINTPTR_MAX == UINT32_MAX
#define DCLANG_FLT   float
typedef int64_t DCLANG_WIDE_INT;
#else
#error "dclang requires 32-bit or 64-bit uintptr_t"
#endif
#define DCLANG_CELL  uintptr_t
#define DCLANG_LONG  long
#define DCLANG_INT   int
#define DCLANG_PTR   uintptr_t
#define DCLANG_ULONG unsigned long
#define DCLANG_UINT  unsigned int
// end of data type macros

#if defined(DCLANG_MATH_NEON)
#include <math_neon.h>
#define DCLANG_SIN sinf_neon
#define DCLANG_COS cosf_neon
#define DCLANG_TAN tanf_neon
#define DCLANG_TANH tanhf_neon
#define DCLANG_EXP expf_neon
#define DCLANG_LOG logf_neon
#define DCLANG_LOG2 log2f
#define DCLANG_LOG10 log10f_neon
#define DCLANG_POW powf_neon
#define DCLANG_SQRT sqrtf_neon
#define DCLANG_FABS fabsf_neon
#define DCLANG_FLOOR floorf_neon
#define DCLANG_CEIL ceilf_neon
#elif UINTPTR_MAX == UINT32_MAX
#define DCLANG_SIN sinf
#define DCLANG_COS cosf
#define DCLANG_TAN tanf
#define DCLANG_TANH tanhf
#define DCLANG_EXP expf
#define DCLANG_LOG logf
#define DCLANG_LOG2 log2f
#define DCLANG_LOG10 log10f
#define DCLANG_POW powf
#define DCLANG_SQRT sqrtf
#define DCLANG_FABS fabsf
#define DCLANG_FLOOR floorf
#define DCLANG_CEIL ceilf
#else
#define DCLANG_SIN sin
#define DCLANG_COS cos
#define DCLANG_TAN tan
#define DCLANG_TANH tanh
#define DCLANG_EXP exp
#define DCLANG_LOG log
#define DCLANG_LOG2 log2
#define DCLANG_LOG10 log10
#define DCLANG_POW pow
#define DCLANG_SQRT sqrt
#define DCLANG_FABS fabs
#define DCLANG_FLOOR floor
#define DCLANG_CEIL ceil
#endif

static inline DCLANG_CELL dclang_float_to_cell(DCLANG_FLT value) {
    DCLANG_CELL cell;
    _Static_assert(sizeof cell == sizeof value,
                   "DCLANG_FLT must occupy exactly one cell");
    memcpy(&cell, &value, sizeof cell);
    return cell;
}

static inline DCLANG_FLT dclang_cell_to_float(DCLANG_CELL cell) {
    DCLANG_FLT value;
    memcpy(&value, &cell, sizeof value);
    return value;
}

// constants
#define DATA_STACK_SIZE   32
#define RETURN_STACK_SIZE 128
#define MAXWORD           1048576
#define EXEC_TRAMPOLINE   (MAXWORD - 4)
#define MAX_OPCODES       272
#define IBUFSIZE          128
#define NUMLOCALS         8
#define NUMVARS           16777216
#define NUM_TREE_ROOTS    32
#define LINE_MAX          128

// function-like macros

#define push(item) { \
    DCLANG_CELL push_value__ = (DCLANG_CELL)(item); \
    if (data_stack_ptr >= DATA_STACK_SIZE) { \
        printf("push -- stack overflow!\n"); \
        data_stack_ptr = 0; \
    } \
    data_stack[data_stack_ptr++] = push_value__; \
}

#define push_no_check(item) { \
    DCLANG_CELL push_value__ = (DCLANG_CELL)(item); \
    data_stack[data_stack_ptr++] = push_value__; \
}

#define POP data_stack[--data_stack_ptr]

#define NEXT goto *dispatch_table[prog[++iptr].opcode]

// compiled tokens get saved and put into an array of type 'inst_struct'
typedef struct {
    int opcode;
    DCLANG_CELL param;
} inst_struct;

// an array of inst_struct instructions. This is where the user's commands,
// i.e. the 'program' will live:
inst_struct  prog[MAXWORD];
DCLANG_PTR   iptr;

// data stack
DCLANG_CELL  data_stack[DATA_STACK_SIZE * 2];
DCLANG_PTR   data_stack_ptr;
DCLANG_PTR   save_data_stack_ptr;
// return stack
DCLANG_LONG  return_stack[RETURN_STACK_SIZE];
DCLANG_PTR   return_stack_ptr;
// loop 'stack'
DCLANG_LONG  loop_counter[3];
DCLANG_PTR   loop_counter_ptr;
// struct for 'for' loops:
typedef struct {
    DCLANG_LONG limit;
    DCLANG_LONG step;
} forloop_info;
forloop_info fl_stack[3];
DCLANG_LONG fl_ptr;
// array for 'times' loop max amounts:
DCLANG_LONG times_info[3];
DCLANG_LONG times_ptr;
// local variables
DCLANG_PTR  locals_keys[NUMLOCALS];
DCLANG_CELL locals_vals[NUMLOCALS * RETURN_STACK_SIZE];
DCLANG_PTR  locals_base_idx;
// global variables
// track variables - mapping from name to index
char        *var_keys[NUMVARS];
DCLANG_PTR   var_vals[NUMVARS];
DCLANG_PTR   var_map_idx;
// variable actual data stored here
DCLANG_CELL  vars[NUMVARS];
DCLANG_PTR   vars_idx;
// global constants
// track constants - mapping from name to index
char        *const_keys[1024];
DCLANG_CELL  const_vals[1024];
DCLANG_PTR   const_idx;

// hashwords
ht *global_hash_table;
char **hashwords;
DCLANG_ULONG hashwords_size = 32;
DCLANG_ULONG hashwords_cnt = 0;

/* main stuct used for a dictionary-style tree entry.
   'key' is a string, 'value' is a double, which can represent
   either a number or a string pointer.

   It is up to the user to supply checks and use the types
   safely. */

struct tree_entry
{
    char *key;
    DCLANG_CELL value;
};

void *tree_roots[NUM_TREE_ROOTS] = {NULL};
int tree_roots_idx = -1;

// Define the structure for linked list nodes
struct Node {
    struct Node *next;
    struct Node *prev;
    DCLANG_CELL data;  // New member for data
};

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
    OP_INC,
    OP_SINC,
    OP_SINCN,
    OP_SSETN,
    OP_SVPUSH,
    OP_SVPOP,
    OP_SVDROP,
    OP_SVPICK,
    OP_SVDEPTH,
    OP_SVCLEAR,
    OP_SHOW,
    OP_SHOWUNSIGNED,
    OP_SHOWFLOAT,
    OP_SHOWNOSPACE,
    OP_SHOWUNSIGNEDNOSPACE,
    OP_SHOWFLOATNOSPACE,
    OP_SHOWINTRJ,
    OP_SHOWUNSIGNEDRJ,
    OP_SHOWRJ,
    OP_SHOWPZ,
    OP_SHOWSTACK,
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
    OP_FEQ,
    OP_FNOTEQ,
    OP_FLT,
    OP_FGT,
    OP_FLTE,
    OP_FGTE,
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
    OP_MULDIV,
    OP_MULDIVMOD,
    OP_DIV,
    OP_MOD,
    OP_DIVMOD,
    OP_ABS,
    OP_MIN,
    OP_MAX,
    OP_FADD,
    OP_FSUB,
    OP_FMUL,
    OP_FDIV,
    OP_FMOD,
    OP_FABS,
    OP_FMIN,
    OP_FMAX,
    // float -> int
    OP_ROUND,
    OP_CEIL,
    OP_FLOOR,
    OP_ITOF,
    OP_FTOI,
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
    OP_SINH,
    OP_COSH,
    OP_TANH,
    // randomness
    OP_RAND,
    // variables and arrays
    OP_VARIABLE,
    OP_PEEK,
    OP_PEEKOFFSET,
    OP_POKE,
    OP_POKE_INC,
    OP_POKEOFFSET_INC,
    OP_POKEOFFSET_INCN,
    OP_POKEOFFSET_SETN,
    OP_ALLOT,
    OP_CREATE,
    OP_COMMA,
    OP_HERE,
    OP_CONSTANT,
    OP_ENVSET,
    OP_ENVGET,
    // sorting
    OP_SORTNUMS,
    OP_SORTFLOATS,
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
    OP_TREEDESTROY,
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
    OP_JUMPZ,
    OP_JUMPU,
    OP_IF,
    OP_ELSE,
    OP_ENDIF,
    OP_CALL,
    OP_SET_LOCAL,
    OP_GET_LOCAL,
    OP_RETURN,
    OP_CLOCK,
    OP_MONOCLOCK,
    _OP_CLOCK_ADD,
    _OP_CLOCK_MOD,
    _OP_SLEEP_UNTIL,
    OP_EPOCH,
    OP_EPOCH_TO_DT,
    OP_DT_TO_EPOCH,
    OP_SLEEP,
    OP_CR,
    OP_PRINT,
    OP_EMIT,
    OP_UEMIT,
    OP_STEREO_OUT32,
    OP_STEREO_IN32,
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
    OP_MARK,
    OP_REWIND,
    OP_MEMUSED,
    // file
    OP_IMPORT,
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
    OP_SETERR,
    OP_SETOUT,
    OP_FLUSH,
    // low-level (OS) file ops:
    OP_OPEN,
    OP_READ,
    OP_WRITE,
    OP_CLOSE,
    // tcp/udp networking using sockets
    OP_TCPLISTEN,
    OP_TCPACCEPT,
    OP_TCPCONNECT,
    OP_UDPSOCKET,
    OP_UDPBIND,
    OP_UDPCONNECT,
    OP_UDPSEND,
    OP_UDPRECV,
    OP_UDPCLOSE,
    // block a SIGINT
    OP_BLOCKSIGINT,
    OP_UNBLOCKSIGINT,
    // os fork and exit
    OP_FORK,
    OP_EXIT,
    // info
    OP_SHOWPRIMITIVES,
    OP_SHOWWORDS,
    OP_SHOWCONSTS,
    OP_SHOWVARS,
    OP_EXEC,
    OP_INPUT,
    // regex
    _OP_REGCOMP,
    _OP_REGEXEC,
    _OP_REGREAD,
    // sqlite3
    _OP_SQLITEOPEN,
    _OP_SQLITEPREPARE,
    _OP_SQLITESTEP,
    _OP_SQLITECOLUMN,
    _OP_SQLITEFINALIZE,
    _OP_SQLITEEXEC,
    _OP_SQLITECLOSE,
    // portmidi
    _OP_PM_LIST,
    _OP_PM_OPENOUT,
    _OP_PM_WS,
    _OP_PM_WSR,
    _OP_PM_OPENIN,
    _OP_PM_READ,
    _OP_PM_CLOSE,
    _OP_PM_TERMINATE,
    _OP_OSC_PUT_STR,
    _OP_OSC_PUT_I32,
    _OP_OSC_PUT_F32,
    _OP_OSC_GET_STR,
    _OP_OSC_GET_I32,
    _OP_OSC_GET_F32,
    _OP_DSP_PHASOR,
    _OP_DSP_DELAY,
    _OP_DSP_REVERB,
    _OP_DSP_FM,
    _OP_DSP_SINELOOP,
    _OP_DSP_TRI,
    _OP_DSP_SAWTOOTH,
    _OP_DSP_WAVETABLE,
    _OP_DSP_PORT,
    _OP_DSP_FAST_SIN,
    _OP_DSP_FAST_TANH,
    _OP_DSP_REVERB_CELLS
};

// user words
typedef struct {
    const char *name;
    DCLANG_PTR word_start;
} user_word;

user_word   user_words[1024];
DCLANG_PTR  num_user_words;

// flag for if we are defining a new word (function)
DCLANG_LONG def_mode;
// flag for if we're in the middle of creating a string
DCLANG_INT in_string;
// input buffer and input file (stdin or file input) stuff
FILE *ifp;
char buf[IBUFSIZE];   // for tokens
DCLANG_LONG bufused;  // for tokens
char line_buf[256] = {0};  // for fgets input
char *line_ptr = line_buf; // for fgets input
DCLANG_INT live_repl = 0;
// a small buffer for use by `OP_INPUT`
char input_pad[512];
int input_here;
// used by 'freadline' function, which calls 'getline'
// must be global so it is accessible to the data stack:
char *linebuf = NULL;
size_t linelen = 0;

// output file, usually stdout, but can be redirected with 'redirect'
FILE *ofp;
// file stack for imports
FILE *file_stack[32];
DCLANG_PTR fsp;

struct timeval tval;
struct timespec dclang_monoclock_origin;
struct sockaddr_in serv_addr, cli_addr;
struct sockaddr_in udp_serv_addr, udp_cli_addr, dest_addr;
sigset_t block_sigint;

// will be the private pointer to the working MIDI stream
PmStream *midi_out_stream, *midi_in_stream;
int midi_initialized;
PmError midi_initialize_status;
#define TIME_PROC ((int32_t (*)(void *)) Pt_Time)
#define TIME_INFO NULL

// Functions we want to define ahead of time
// Helps us avoid chicken-or-egg problems
void _execfunc(void);
void (*execfunc)(void);
void _show_primitives(void);
void (*show_primitives)(void);
void _repl(void);
void (*repl)(void);
void dclang_execute(void);

///////////////
// API calls //
///////////////

extern void dclang_initialize();
extern DCLANG_LONG dclang_import(char *infilestr);
extern DCLANG_LONG dclang_findword(const char *word);
extern void dclang_callword(DCLANG_PTR where);
extern DCLANG_CELL dclang_pop();
