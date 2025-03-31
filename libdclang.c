/* dclang, an RPN programming language.

'True refinement seeks simplicity.' - Bruce Lee

Born on 2018-05-05 */

#include "dclang.h"
#include "token.c"

// BEGIN HELPERS

// Input pointer handling

void setinput(FILE *fp) {
    file_stack[fsp++] = ifp;
    ifp = fp;
}

void revertinput() {
    if (fsp == 0) {
        exit(0);
    }
    ifp = file_stack[--fsp];
}

////////////////////////////
// Dealing with importing //
////////////////////////////
DCLANG_LONG dclang_import(char *infilestr) {
    DCLANG_INT saved_live_repl = live_repl;
    live_repl = 0;
    char *prefix = getenv("DCLANG_LIBS");
    if (prefix == NULL) {
        printf("DCLANG_LIBS env variable is unset!\n");
        return -1;
    }
    // check existence of file:
    if (access(infilestr, F_OK) == 0) {
        FILE *infile;
        infile = fopen(infilestr, "r");
        setinput(infile);
        repl();
        live_repl = saved_live_repl;
        return 0;
    }
    char *full_path = dclang_malloc(512);
    memset(full_path, 0, 512);
    char *slash = "/";
    strcat(full_path, prefix);
    strcat(full_path, slash);
    strcat(full_path, infilestr);
    if (access(full_path, F_OK) == 0) {
        FILE *infile = fopen(full_path, "r");
        setinput(infile);
        repl();
        live_repl = saved_live_repl;
        return 0;
    }
    printf(
        "The file named %s doesn't appear to exist in the current " \
        "directory, or under %s as %s, or cannot be accessed.\n"
        "You may want to check its existence and permissions!\n",
        infilestr, prefix, full_path
    );
    return -1;
}

DCLANG_FLT dclang_pop() {
// special case -- has a return value b/c it can
// be used in other calls to give a value, which
// also means it can be an API call.
    return data_stack[--data_stack_ptr];
}

// helper functions for sorting

int compare_doubles (const void *a, const void *b) {
    const double *da = (const double *) a;
    const double *db = (const double *) b;
    return (*da > *db) - (*da < *db);
}

int compare_strings (const void *a, const void *b) {
    const char *sa = (const char *) (DCLANG_PTR) * (DCLANG_FLT *) a;
    const char *sb = (const char *) (DCLANG_PTR) * (DCLANG_FLT *) b;
    return strcmp(sa, sb);
}

// end helper functions for sorting

// helper functions for trees

struct tree_entry *
make_tree_entry(char *key, DCLANG_FLT value) {
    struct tree_entry *new_tree =
        (struct tree_entry *) dclang_malloc(sizeof(struct tree_entry));
    if(!new_tree) {
        printf("make_tree_entry malloc fail\n");
        exit(1);
    }
    new_tree->key = dclang_strdup(key);
    new_tree->value = value;
    return new_tree;
}

int tree_compare_func(const void *l, const void *r) {
    if (l == NULL || r == NULL) return 0;
    struct tree_entry *tree_l = (struct tree_entry *)l;
    struct tree_entry *tree_r = (struct tree_entry *)r;
    return strcmp(tree_l->key, tree_r->key);
}

// helper used by `treewalk`
void print_node(const void *node, const VISIT order, const int depth) {
    if (order == preorder || order == leaf ) {
		    printf(
		        "key=%s, value=%s\n",
		        (*(struct tree_entry **)node)->key,
		        (char *)(DCLANG_PTR)((*(struct tree_entry **)node)->value)
		    );
    }
}

// end helper functions for trees

///////////////////////////////////////////
// Helpers for local variable processing //
///////////////////////////////////////////
// Function to swap two cells
void swap(DCLANG_PTR *a, DCLANG_PTR *b) {
    DCLANG_PTR temp = *a;
    *a = *b;
    *b = temp;
}

// function to reverse an array
void reverse_array(DCLANG_PTR arr[], int n) {
    // Initialize left to the beginning and right to the end
    int left = 0, right = n;
    // Iterate till left is less than right
    while (left < right) {
        // Swap the elements at left and right position
        swap(&arr[left], &arr[right]);
        // Increment the left pointer
        left++;
        // Decrement the right pointer
        right--;
    }
}
// end locals helpers


// sqlite3 API helper
static int __sqlcallback(void *NotUsed, int argc, char **argv, char **azColName) {
    int i;
    for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

// END HELPERS

void dclang_execute() {
    char *char_ptr, *env_key, *env_val, *key, *search_key, *str, *str1, *str2, *buf, *mode, *path, *host, *sql;
    regex_t *regex;
    struct tree_entry *te, *te_del, *retval;
    struct Node *list, *next, *node_to_remove, *tail_node, *node, *new_node, *node_before;
    struct hostent *server;
    sqlite3 *db;
    sqlite3_stmt *stmt;
    void *hvalue, *confirm;
    FILE *file;
    DCLANG_INT arrsize, precision, width, bufsize, i, ch, regex_flags, fd, sockfd, portno, rc;
    DCLANG_FLT value, val, val1, val2, a, b, c, tree_val;
    DCLANG_LONG slot, truth, result, midi_status, midi_data1, midi_data2;
    DCLANG_PTR arrstart, env_key_addr, key_addr, idx, next_var, size, tree_idx, \
               list_ptr, str_ptr_addr, str_ptr_addr2, dest, delim, buflen, strpt, bufaddr, num_bytes;
    DCLANG_ULONG shift_amt, base, fmt;

    static void *dispatch_table[] = {
        &&OP_NOOP,
        &&OP_PUSH,
        &&OP_PUSH_NO_CHECK,
        &&OP_DROP,
        &&OP_DUP,
        &&OP_OVER,
        &&OP_PICK,
        &&OP_SWAP,
        &&OP_DROP2,
        &&OP_DUP2,
        &&OP_OVER2,
        &&OP_DEPTH,
        &&OP_CLEAR,
        &&OP_SVPUSH,
        &&OP_SVPOP,
        &&OP_SVDROP,
        &&OP_SVPICK,
        &&OP_SVDEPTH,
        &&OP_SVCLEAR,
        &&OP_SHOW,
        &&OP_SHOWNOSPACE,
        &&OP_SHOWRJ,
        &&OP_SHOWPZ,
        &&OP_SHOWSTACK,
        &&OP_NULL,
        &&OP_FALSE,
        &&OP_TRUE,
        &&OP_EQ,
        &&OP_NOTEQ,
        &&OP_LT,
        &&OP_GT,
        &&OP_LTE,
        &&OP_GTE,
        &&OP_ASSERT,
        &&OP_AND,
        &&OP_OR,
        &&OP_XOR,
        &&OP_NOT,
        &&OP_LSHIFT,
        &&OP_RSHIFT,
        &&OP_ADD,
        &&OP_SUB,
        &&OP_MUL,
        &&OP_DIV,
        &&OP_MOD,
        &&OP_ABS,
        &&OP_MIN,
        &&OP_MAX,
        &&OP_ROUND,
        &&OP_CEIL,
        &&OP_FLOOR,
        &&OP_POWER,
        &&OP_SQRT,
        &&OP_EXP,
        &&OP_LOG,
        &&OP_LOG2,
        &&OP_LOG10,
        &&OP_E,
        &&OP_PI,
        &&OP_SINE,
        &&OP_COS,
        &&OP_TAN,
        &&OP_ASINE,
        &&OP_ACOS,
        &&OP_ATAN,
        &&OP_ATAN2,
        &&OP_SINH,
        &&OP_COSH,
        &&OP_TANH,
        &&OP_RAND,
        &&OP_VARIABLE,
        &&OP_POKE,
        &&OP_PEEK,
        &&OP_ALLOT,
        &&OP_CREATE,
        &&OP_COMMA,
        &&OP_HERE,
        &&OP_CONSTANT,
        &&OP_ENVSET,
        &&OP_ENVGET,
        &&OP_SORTNUMS,
        &&OP_SORTSTRS,
        &&OP_HASHSET,
        &&OP_HASHGET,
        &&OP_HASHKEYS,
        &&OP_TREEMAKE,
        &&OP_TREESET,
        &&OP_TREEGET,
        &&OP_TREEWALK,
        &&OP_TREEDELETE,
    #ifdef HAS_TREEDESTROY
        &&OP_TREEDESTROY,
    #endif
        &&OP_LISTMAKE,
        &&OP_LISTNEXT,
        &&OP_LISTSET,
        &&OP_LISTGET,
        &&OP_LISTPUSH,
        &&OP_LISTPOP,
        &&OP_LISTINSERT,
        &&OP_LISTREMOVE,
        &&OP_LISTSIZE,
        &&OP_LISTDELETE,
        &&OP_TIMES,
        &&OP_AGAIN,
        &&OP_EXITTIMES,
        &&OP_FOR,
        &&OP_NEXT,
        &&OP_EXITFOR,
        &&OP_I,
        &&OP_J,
        &&OP_K,
        &&OP_JUMPZ,
        &&OP_JUMPU,
        &&OP_IF,
        &&OP_ELSE,
        &&OP_ENDIF,
        &&OP_CALL,
        &&OP_SET_LOCAL,
        &&OP_GET_LOCAL,
        &&OP_RETURN,
        &&OP_CLOCK,
        &&OP_EPOCH,
        &&OP_EPOCH_TO_DT,
        &&OP_DT_TO_EPOCH,
        &&OP_SLEEP,
        &&OP_CR,
        &&OP_PRINT,
        &&OP_EMIT,
        &&OP_UEMIT,
        &&OP_BYTES32,
        &&OP_ISALNUM,
        &&OP_ISALPHA,
        &&OP_ISCNTRL,
        &&OP_ISDIGIT,
        &&OP_ISGRAPH,
        &&OP_ISLOWER,
        &&OP_ISPRINT,
        &&OP_ISPUNCT,
        &&OP_ISSPACE,
        &&OP_ISUPPER,
        &&OP_ISXDIGIT,
        &&OP_TOHEX,
        &&OP_TOSTR,
        &&OP_TONUM,
        &&OP_ORD,
        &&OP_TOLOWER,
        &&OP_TOUPPER,
        &&OP_STRLEN,
        &&OP_STREQ,
        &&OP_STRLT,
        &&OP_STRGT,
        &&OP_STRFIND,
        &&OP_STRSPN,
        &&OP_STRCSPN,
        &&OP_STRTOK,
        &&OP_MEMPCPY,
        &&OP_MEMSET,
        &&OP_MKBUF,
        &&OP_FREE,
        &&OP_MEMUSED,
        &&OP_REGCOMP,
        &&OP_REGEXEC,
        &&OP_REGREAD,
        &&OP_IMPORT,
        &&OP_FILEOPEN,
        &&OP_FILEMEMOPEN,
        &&OP_FILEREAD,
        &&OP_FILEREADLINE,
        &&OP_FILEREADALL,
        &&OP_FILESEEK,
        &&OP_FILETELL,
        &&OP_FILEWRITE,
        &&OP_FILEFLUSH,
        &&OP_FILECLOSE,
        &&OP_REDIRECT,
        &&OP_RESETOUT,
        &&OP_FLUSHOUT,
        &&OP_OPEN,
        &&OP_READ,
        &&OP_WRITE,
        &&OP_CLOSE,
        &&OP_TCPLISTEN,
        &&OP_TCPACCEPT,
        &&OP_TCPCONNECT,
        &&OP_UDPRECV,
        &&OP_UDPSEND,
        &&OP_BLOCKSIGINT,
        &&OP_UNBLOCKSIGINT,
        &&OP_FORK,
        &&OP_EXIT,
        &&OP_SHOWPRIMITIVES,
        &&OP_SHOWWORDS,
        &&OP_SHOWCONSTS,
        &&OP_SHOWVARS,
        &&OP_EXEC,
        &&OP_INPUT,
        &&_OP_SQLITEOPEN,
        &&_OP_SQLITEPREPARE,
        &&_OP_SQLITESTEP,
        &&_OP_SQLITECOLUMN,
        &&_OP_SQLITEFINALIZE,
        &&_OP_SQLITEEXEC,
        &&_OP_SQLITECLOSE,
        &&_OP_PM_LIST,
        &&_OP_PM_OPENOUT,
        &&_OP_PM_WS,
        &&_OP_PM_WSR,
        &&_OP_PM_CLOSE,
        &&_OP_PM_TERMINATE
    };

    while (1) {
        goto *dispatch_table[prog[iptr].opcode];

        OP_NOOP:
            return;
        // stack ops
        OP_PUSH:
            if (data_stack_ptr >= DATA_STACK_SIZE) {
                printf("push -- stack overflow!\n");
                data_stack_ptr = 0;
            }
            data_stack[data_stack_ptr++] = prog[iptr].param;
            NEXT;
        OP_PUSH_NO_CHECK:
            data_stack[data_stack_ptr++] = prog[iptr].param;
            NEXT;
        OP_DROP:
            if (data_stack_ptr < 1) {
                printf("drop -- stack underflow!\n");
                return;
            }
            --data_stack_ptr;
            NEXT;
        OP_DUP:
            if (data_stack_ptr < 1) {
                printf("dup -- stack underflow!\n");
                return;
            }
            push(data_stack[data_stack_ptr - 1]);
            NEXT;
        OP_OVER:
            if (data_stack_ptr < 2) {
                printf("over -- stack underflow!\n");
                return;
            }
            push(data_stack[data_stack_ptr - 2]);
            NEXT;
        OP_PICK:
            if (data_stack_ptr < 1) {
                 printf("pick -- stack underflow!\n");
                 return;
            }
            DCLANG_PTR pick_idx = (DCLANG_PTR) POP;
            if (data_stack_ptr < (pick_idx + 1)) {
                printf("pick -- stack not deep enough!\n");
                return;
            }
            val = data_stack[data_stack_ptr - (pick_idx + 1)];
            data_stack[data_stack_ptr++] = val;
            NEXT;
        OP_SWAP:
            if (data_stack_ptr < 2) {
                printf("swap -- stack underflow!\n");
                return;
            }
            val1 = data_stack[--data_stack_ptr];
            val2 = data_stack[--data_stack_ptr];
            data_stack[data_stack_ptr++] = val1;
            data_stack[data_stack_ptr++] = val2;
            NEXT;
        OP_DROP2:
            if (data_stack_ptr < 2) {
                printf("2drop -- stack underflow!\n");
                return;
            }
            --data_stack_ptr;
            --data_stack_ptr;
            NEXT;
        OP_DUP2:
            if (data_stack_ptr < 2) {
                printf("2dup -- stack underflow!\n");
                return;
            }
            val2 = data_stack[data_stack_ptr - 1];
            push(data_stack[data_stack_ptr - 2]);
            push(data_stack[data_stack_ptr - 2]);
            NEXT;
        OP_OVER2:
            if (data_stack_ptr < 4) {
                printf("2over -- stack underflow!\n");
                return;
            }
            push(data_stack[data_stack_ptr - 4]);
            push(data_stack[data_stack_ptr - 4]);
            NEXT;
        OP_DEPTH:
            size = data_stack_ptr;
            push((DCLANG_FLT)size);
            NEXT;
        OP_CLEAR:
            // clears the stack!
            data_stack_ptr = 0;
            NEXT;
        // save-stack ops
        OP_SVPUSH:
            if (save_data_stack_ptr >= DATA_STACK_SIZE) {
                printf("svpush -- stack overflow!\n");
                save_data_stack_ptr = 0;
            }
            val = POP;
            data_stack[save_data_stack_ptr++ + DATA_STACK_SIZE] = val;
            NEXT;
        OP_SVPOP:
            if (save_data_stack_ptr <= 0) {
                printf("svdrop -- stack underflow!\n");
                save_data_stack_ptr = 0;
                return;
            }
            val = data_stack[--save_data_stack_ptr + DATA_STACK_SIZE];
            data_stack[data_stack_ptr++] = val;
            NEXT;
        OP_SVDROP:
            if (save_data_stack_ptr <= 0) {
                printf("svdrop -- stack underflow!\n");
                return;
            }
            --save_data_stack_ptr;
            NEXT;
        OP_SVPICK:
            if (save_data_stack_ptr <= 0) {
                 printf("svpick -- stack underflow!\n");
                 return;
            }
            DCLANG_PTR svpick_idx = (DCLANG_PTR) POP;
            if (save_data_stack_ptr < (svpick_idx + 1)) {
                printf("svpick -- stack not deep enough!\n");
                return;
            }
            val = data_stack[(save_data_stack_ptr - (svpick_idx + 1)) + DATA_STACK_SIZE];
            data_stack[data_stack_ptr++] = val;
            NEXT;
        OP_SVDEPTH:
            size = save_data_stack_ptr;
            data_stack[data_stack_ptr++] = (DCLANG_FLT)size;
            NEXT;
        OP_SVCLEAR:
            // clears the stack!
            save_data_stack_ptr = 0;
            NEXT;
        // stack display primitives
        OP_SHOW:
            if (data_stack_ptr < 1) {
                printf(". (pop) -- stack underflow! ");
                return;
            }
            val = data_stack[--data_stack_ptr];
            fprintf(ofp, "%0.19g ", val);
            fflush(ofp);
            NEXT;
        OP_SHOWNOSPACE:
            if (data_stack_ptr < 1) {
                printf("stack underflow! ");
                return;
            }
            val = data_stack[--data_stack_ptr];
            fprintf(ofp, "%0.19g", val);
            fflush(ofp);
            NEXT;
        OP_SHOWRJ:
            if (data_stack_ptr < 3) {
                printf("Stack underflow! '.rj' needs: value, width, precision on the stack\n");
                return;
            }
            // right-justified for pretty printing!
            precision = (DCLANG_LONG) POP;
            width = (DCLANG_LONG) POP;
            fprintf(ofp, "%*.*f ", width, precision, POP);
            fflush(ofp);
            NEXT;
        OP_SHOWPZ:
            // left pad with zeros
            if (data_stack_ptr < 3) {
                printf("Stack underflow! '.pz' needs: value, width, precision on the stack\n");
                return;
            }
            precision = (DCLANG_LONG) POP;
            width = (DCLANG_LONG) POP;
            fprintf(ofp, "%0*.*f ", width, precision, POP);
            fflush(ofp);
            NEXT;
        OP_SHOWSTACK:
            DCLANG_LONG shx;
            char *joiner;
            shx = data_stack_ptr > 16 ? data_stack_ptr - 16 : 0;
            joiner = shx == 0 ? " " : " ... ";
            fprintf(ofp, "data_stack: <%lu>%s", data_stack_ptr, joiner);
            fflush(ofp);
            for (shx=0; shx < data_stack_ptr; shx++) {
                fprintf(ofp, "%0.19g ", data_stack[shx]);
                fflush(ofp);
            }
            fprintf(ofp, "\n");
            // do the save data stack as well:
            DCLANG_LONG shy;
            char *sv_joiner;
            shy = save_data_stack_ptr > 16 ? save_data_stack_ptr - 16 : 0;
            sv_joiner = shy == 0 ? " " : " ... ";
            fprintf(ofp, "save_stack: <%lu>%s", save_data_stack_ptr, sv_joiner);
            fflush(ofp);
            for (shy=0; shy < save_data_stack_ptr; shy++) {
                fprintf(ofp, "%0.19g ", data_stack[shy + DATA_STACK_SIZE]);
                fflush(ofp);
            }
            fprintf(ofp, "\n");
            NEXT;
        // true/false syntactic sugar
        OP_NULL:
            void *ptr = NULL;
            push((DCLANG_LONG)ptr);
            NEXT;
        OP_FALSE:
            push(0);
            NEXT;
        OP_TRUE:
            push(-1);
            NEXT;
        // comparison booleans
        OP_EQ:
            if (data_stack_ptr < 2)
            {
                printf("= -- stack underflow; needs <val1> <val2> on the stack!\n");
                return;
            }
            push(((DCLANG_FLT) POP == (DCLANG_FLT) POP) * -1);
            NEXT;
        OP_NOTEQ:
            if (data_stack_ptr < 2)
            {
                printf("!= -- stack underflow; needs <val1> <val2> on the stack!\n");
                return;
            }
            push(((DCLANG_FLT) POP != (DCLANG_FLT) POP) * -1);
            NEXT;
        OP_GT:
            if (data_stack_ptr < 2)
            {
                printf("> -- stack underflow; needs <val1> <val2> on the stack!\n");
                return;
            }
            push(((DCLANG_FLT) POP < (DCLANG_FLT) POP) * -1);
            NEXT;
        OP_LT:
            if (data_stack_ptr < 2)
            {
                printf("< -- stack underflow; needs <val1> <val2> on the stack!\n");
                return;
            }
            push(((DCLANG_FLT) POP > (DCLANG_FLT) POP) * -1);
            NEXT;
        OP_GTE:
            if (data_stack_ptr < 2)
            {
                printf(">= -- stack underflow; needs <val1> <val2> on the stack!\n");
                return;
            }
            push(((DCLANG_FLT) POP <= (DCLANG_FLT) POP) * -1);
            NEXT;
        OP_LTE:
            if (data_stack_ptr < 2)
            {
                printf("<= -- stack underflow; needs <val1> <val2> on the stack!\n");
                return;
            }
            push(((DCLANG_FLT) POP >= (DCLANG_FLT) POP) * -1);
            NEXT;
        // assertions
        OP_ASSERT:
            if (data_stack_ptr < 1)
            {
                printf("assert -- stack underflow; needs <bool> on the stack!\n");
                return;
            }
            truth = POP;
            if (truth == 0) {
                printf("ASSERT FAIL!\n");
            }
            NEXT;
        // bitwise operators
        OP_AND:
            if (data_stack_ptr < 2)
            {
                printf("and -- stack underflow; needs <int1> <int2> on the stack!\n");
                return;
            }
            push((DCLANG_LONG) POP & (DCLANG_LONG) POP);
            NEXT;
        OP_OR:
            if (data_stack_ptr < 2)
            {
                printf("or -- stack underflow; needs <int1> <int2> on the stack!\n");
                return;
            }
            push((DCLANG_LONG) POP | (DCLANG_LONG) POP);
            NEXT;
        OP_XOR:
            if (data_stack_ptr < 2)
            {
                printf("xor -- stack underflow; needs <int1> <int2> on the stack!\n");
                return;
            }
            push((DCLANG_LONG) POP ^ (DCLANG_LONG) POP);
            NEXT;
        OP_NOT:
            if (data_stack_ptr < 1)
            {
                printf("not -- stack underflow; needs <int> on the stack!\n");
                return;
            }
            push(~(DCLANG_LONG) POP);
            NEXT;
        OP_LSHIFT:
            if (data_stack_ptr < 2)
            {
                printf("<< -- stack underflow; needs <int1> <int2> on the stack!\n");
                return;
            }
            shift_amt = (DCLANG_ULONG) POP;
            base = (DCLANG_ULONG) POP;
            push((DCLANG_ULONG) base << shift_amt);
            NEXT;
        OP_RSHIFT:
            if (data_stack_ptr < 2)
            {
                printf(">> -- stack underflow; needs <int1> <int2> on the stack!\n");
                return;
            }
            shift_amt = (DCLANG_ULONG) POP;
            base = (DCLANG_ULONG) POP;
            push((DCLANG_ULONG) base >> shift_amt);
            NEXT;
        // math
        OP_ADD:
            if (data_stack_ptr < 2)
            {
                printf("+ -- stack underflow; needs <num1> <num2> on the stack!\n");
                return;
            }
            push(POP + POP);
            NEXT;
        OP_SUB:
            if (data_stack_ptr < 2)
            {
                printf("- -- stack underflow; needs <num1> <num2> on the stack!\n");
                return;
            }
            DCLANG_FLT subtrahend = POP;
            push(POP - subtrahend);
            NEXT;
        OP_MUL:
            if (data_stack_ptr < 2)
            {
                printf("* -- stack underflow; needs <num1> <num2> on the stack!\n");
                return;
            }
            push(POP * POP);
            NEXT;
        OP_DIV:
            if (data_stack_ptr < 2)
            {
                printf("/ -- stack underflow; needs <num1> <num2> on the stack!\n");
                return;
            }
            DCLANG_FLT divisor = POP;
            push(POP / divisor);
            NEXT;
        OP_MOD:
            if (data_stack_ptr < 2)
            {
                printf("%% -- stack underflow; needs <num1> <num2> on the stack!\n");
                return;
            }
            DCLANG_FLT modulus = POP;
            push(fmod(POP, modulus));
            NEXT;
        OP_ABS:
            if (data_stack_ptr < 1)
            {
                printf("abs -- stack underflow; needs <num> on the stack!\n");
                return;
            }
            push(fabs(POP));
            NEXT;
        OP_MIN:
            if (data_stack_ptr < 2)
            {
                printf("min -- stack underflow; needs <num1> <num2> on the stack!\n");
                return;
            }
            a = POP;
            b = POP;
            c = (a < b) ? a : b;
            push(c);
            NEXT;
        OP_MAX:
            if (data_stack_ptr < 2)
            {
                printf("max -- stack underflow; needs <num1> <num2> on the stack!\n");
                return;
            }
            a = POP;
            b = POP;
            c = (a > b) ? a : b;
            push(c);
            NEXT;
        OP_ROUND:
            if (data_stack_ptr < 1)
            {
                printf("round needs a number on the stack!\n");
                return;
            }
            push((DCLANG_LONG) round(POP));
            NEXT;
        OP_FLOOR:
            if (data_stack_ptr < 1)
            {
                printf("floor needs a number on the stack!\n");
                return;
            }
            push((DCLANG_LONG) floor(POP));
            NEXT;
        OP_CEIL:
            if (data_stack_ptr < 1)
            {
                printf("ceil needs a number on the stack!\n");
                return;
            }
            push((DCLANG_LONG) ceil(POP));
            NEXT;
        // scientific math words
        OP_POWER:
            if (data_stack_ptr < 2)
            {
                printf("pow -- stack underflow; needs <num1> <num2> on the stack!\n");
                return;
            }
            DCLANG_FLT raise = POP;
            push(pow(POP, raise));
            NEXT;
        OP_SQRT:
            if (data_stack_ptr < 1)
            {
                printf("sqrt needs a number on the stack!\n");
                return;
            }
            push(sqrt(POP));
            NEXT;
        OP_EXP:
            if (data_stack_ptr < 1)
            {
                printf("exp needs a number on the stack!\n");
                return;
            }
            push(exp(POP));
            NEXT;
        OP_LOG:
            if (data_stack_ptr < 1)
            {
                printf("log needs a number on the stack!\n");
                return;
            }
            push(log(POP));
            NEXT;
        OP_LOG2:
            if (data_stack_ptr < 1)
            {
                printf("log2 needs a number on the stack!\n");
                return;
            }
            push(log2(POP));
            NEXT;
        OP_LOG10:
            if (data_stack_ptr < 1)
            {
                printf("log10 needs a number on the stack!\n");
                return;
            }
            push(log10(POP));
            NEXT;
        OP_E:
            push(M_E);
            NEXT;
        // Trig, pi, etc.
        OP_PI:
            push(M_PI);
            NEXT;
        OP_SINE:
            if (data_stack_ptr < 1)
            {
                printf("sin needs a number on the stack!\n");
                return;
            }
            push(sin(POP));
            NEXT;
        OP_COS:
            if (data_stack_ptr < 1)
            {
                printf("cos needs a number on the stack!\n");
                return;
            }
            push(cos(POP));
            NEXT;
        OP_TAN:
            if (data_stack_ptr < 1)
            {
                printf("tan needs a number on the stack!\n");
                return;
            }
            push(tan(POP));
            NEXT;
        OP_ASINE:
            if (data_stack_ptr < 1)
            {
                printf("asin needs a number on the stack!\n");
                return;
            }
            push(asin(POP));
            NEXT;
        OP_ACOS:
            if (data_stack_ptr < 1)
            {
                printf("acos needs a number on the stack!\n");
                return;
            }
            push(acos(POP));
            NEXT;
        OP_ATAN:
            if (data_stack_ptr < 1)
            {
                printf("atan needs a number on the stack!\n");
                return;
            }
            push(atan(POP));
            NEXT;
        OP_ATAN2:
            if (data_stack_ptr < 2)
            {
                printf("atan2 -- stack underflow; needs <num1> <num2> on the stack!\n");
                return;
            }
            DCLANG_FLT x = POP;
            DCLANG_FLT y = POP;
            push(atan2(y, x));
            NEXT;
        OP_SINH:
            if (data_stack_ptr < 1)
            {
                printf("sinh needs a number on the stack!\n");
                return;
            }
            push(sinh(POP));
            NEXT;
        OP_COSH:
            if (data_stack_ptr < 1)
            {
                printf("cosh needs a number on the stack!\n");
                return;
            }
            push(cosh(POP));
            NEXT;
        OP_TANH:
            if (data_stack_ptr < 1)
            {
                printf("tanh needs a number on the stack!\n");
                return;
            }
            push(tanh(POP));
            NEXT;
        // randomness
        OP_RAND:
            push((double)rand()/(double)RAND_MAX);
            NEXT;
        // variables
        OP_VARIABLE:
            next_var = vars_idx++;
            var_keys[var_map_idx] = get_token();
            var_vals[var_map_idx++] = next_var;
            NEXT;
        OP_POKE:
            if (data_stack_ptr < 2)
            {
                printf("! -- stack underflow; needs <val> <variable> on the stack! ");
                return;
            }
            idx = (DCLANG_PTR) POP;
            if (idx < 0 || idx > NUMVARS)
            {
                printf("! -- variable slot number out-of-range!\n");
                return;
            }
            val = POP;
            vars[idx] = val;
            NEXT;
        OP_PEEK:
            if (data_stack_ptr < 1)
            {
                printf("@ -- stack underflow; needs <variable> on the stack! ");
                return;
            }
            idx = (DCLANG_PTR) POP;
            if (idx < 0 || idx > NUMVARS)
            {
                printf("@ -- variable slot number %lu is out-of-range!\n", idx);
                return;
            }
            push(vars[idx]);
            NEXT;
        OP_ALLOT:
            if (data_stack_ptr < 1)
            {
                printf("allot -- stack underflow; needs <num-of-slots> value on the stack! ");
                return;
            }
            vars_idx += (DCLANG_PTR) POP - 1;
            NEXT;
        OP_CREATE:
            next_var = vars_idx++;
            var_keys[var_map_idx] = get_token();
            var_vals[var_map_idx++] = next_var;
            --vars_idx;
            NEXT;
        OP_COMMA:
            if (data_stack_ptr < 1)
            {
                printf(", -- stack underflow; needs a <num> to push into a slot! ");
                return;
            }
            val = POP;
            vars[vars_idx++] = val;
            NEXT;
        OP_HERE:
            push((DCLANG_PTR) vars_idx);
            NEXT;
        OP_CONSTANT:
            const_keys[const_idx] = get_token();
            const_vals[const_idx++] = POP;
            NEXT;
        // environment variables
        OP_ENVSET:
            if (data_stack_ptr < 2)
            {
                printf("envset -- needs <env_val> <env_key> strings on the stack.\n");
            }
            // grab the key from the stack
            env_key = (char *)(DCLANG_PTR) POP;
            env_key_addr = (DCLANG_PTR) env_key;
            if (env_key_addr < MIN_STR || env_key_addr > MAX_STR)
            {
                perror("envset -- String address for environment key out-of-range.");
                return;
            }
            // grab the value from the stack
            env_val = (char *)(DCLANG_PTR) POP;
            DCLANG_PTR env_val_addr = (DCLANG_PTR) env_val;
            if (env_val_addr < MIN_STR || env_val_addr > MAX_STR)
            {
                perror("envset -- String address for environment value out-of-range.");
                return;
            }
            // set the key's value
            setenv(env_key, env_val, 1);
            // no value put on stack -- only side effect
            NEXT;
        OP_ENVGET:
            if (data_stack_ptr < 1)
            {
                printf("envget -- needs <env_key> string on the stack.\n");
            }
            // grab the key
            env_key = (char *)(DCLANG_PTR) POP;
            env_key_addr = (DCLANG_PTR) env_key;
            if (env_key_addr < MIN_STR || env_key_addr > MAX_STR)
            {
                perror("envget -- String address for hash key out-of-range.");
                return;
            }
            env_val = getenv(env_key);
            DCLANG_PTR val_addr = (DCLANG_PTR) env_val;
            if (val_addr > MAX_STR || MAX_STR == 0)
            {
                MAX_STR = val_addr;
            }
            if ((val_addr != 0) && (val_addr < MIN_STR || MIN_STR == 0))
            {
                MIN_STR = val_addr;
            }
            push(val_addr);
            NEXT;
        // sorting
        OP_SORTNUMS:
            if (data_stack_ptr < 2)
            {
                printf("sortnums -- needs <arrstart_index> <size> on the stack.\n");
                return;
            }
            arrsize = (DCLANG_INT) POP;
            arrstart = (DCLANG_PTR) POP;
            qsort((void *)(vars+arrstart), arrsize, sizeof(DCLANG_FLT), compare_doubles);
            NEXT;
        OP_SORTSTRS:
            if (data_stack_ptr < 2)
            {
                printf("sortstrs -- needs <arrstart_index> <size> on the stack.\n");
            }
            arrsize = (DCLANG_PTR) POP;
            arrstart = (DCLANG_PTR) POP;
            qsort((void *)(vars+arrstart), arrsize, sizeof(DCLANG_FLT), compare_strings);
            NEXT;
        // global hash table
        OP_HASHSET:
            if (data_stack_ptr < 2)
            {
                printf("h! -- stack underflow; needs <value> <key-string> on the stack! ");
                return;
            }
            // grab the key
            key = (const char *)(DCLANG_PTR) POP;
            key_addr = (DCLANG_PTR) key;
            if (key_addr < MIN_STR || key_addr > MAX_STR)
            {
                perror("h! -- String address for hash key out-of-range.");
                return;
            }
            // grab the value
            hvalue = (void *)(DCLANG_PTR) POP;
            confirm = hset(global_hash_table, key, hvalue);
            if (confirm != NULL)
            {
                if (hashwords_cnt > hashwords_size)
                {
                    hashwords_size *= 2;
                    hashwords = (char **)dclang_realloc(hashwords, hashwords_size * sizeof(*hashwords));
                }
                hashwords[hashwords_cnt] = key;
                ++hashwords_cnt;
            }
            NEXT;
        OP_HASHGET:
            if (data_stack_ptr < 1)
            {
                printf("h@ -- stack underflow; needs a <key-string> on the stack! ");
                return;
            }
            // grab the key
            key = (char *)(DCLANG_PTR) POP;
            key_addr = (DCLANG_PTR) key;
            if (key_addr < MIN_STR || key_addr > MAX_STR)
            {
                perror("h@ -- String address for hash key out-of-range.");
                return;
            }
            // grab the value
            hvalue = hget(global_hash_table, key);
            if (hvalue == NULL)
            {
                push(0);
            } else {
                push((DCLANG_PTR)(char *)hvalue);
            }
            NEXT;
        OP_HASHKEYS:
            if (data_stack_ptr < 1)
            {
                printf("hkeys -- stack underflow; needs an <key_idx> on stack! ");
                return;
            }
            // grab the key index
            DCLANG_PTR keyidx = (DCLANG_PTR) POP;
            push((DCLANG_PTR) hashwords[keyidx]);
            NEXT;
        // trees
        OP_TREEMAKE:
            tree_roots_idx += 1;
            if (tree_roots_idx > (NUM_TREE_ROOTS - 1))
            {
                printf("tmake -- tree root limit reached!\n");
                printf("You can change the limit when compiling via the NUM_TREE_ROOTS variable\n");
                return;
            }
            push((DCLANG_PTR) tree_roots_idx);
            NEXT;
        OP_TREEGET:
            if (data_stack_ptr < 2)
            {
                printf("t@ -- stack underflow! needs <tree_root> <keystr> on the stack.\n");
                printf("You can make a root node via 'tmake', and assign it to a variable ");
                printf("so it can be referred to later.\n");
                return;
            }
            // Pop args
            search_key = (char *)(DCLANG_PTR) POP;
            tree_idx = (DCLANG_PTR) POP;
            struct tree_entry dummy_tree;
            dummy_tree.key = search_key;
            dummy_tree.value = 0;
            retval = tfind(&dummy_tree, &tree_roots[tree_idx], tree_compare_func);
            if (retval == NULL)
            {
                push((DCLANG_PTR) 0);
                return;
            }
            push((DCLANG_FLT)((*(struct tree_entry **)retval)->value));
            NEXT;
        OP_TREESET:
            if (data_stack_ptr < 3)
            {
                printf("t! -- stack underflow! needs <tree_root> <keystr> <val> on the stack.\n");
                printf("You can make a root node via 'tmake', and assign it to a variable ");
                printf("so it can be referred to later.\n");
                return;
            }
            // Pop args
            tree_val = (DCLANG_FLT) POP;
            search_key = (char *)(DCLANG_PTR) POP;
            tree_idx = (DCLANG_PTR) POP;
            te_del = make_tree_entry(dclang_strdup(search_key), tree_val);
            tdelete(te_del, &tree_roots[tree_idx], tree_compare_func);
            te = make_tree_entry(dclang_strdup(search_key), tree_val);
            retval = tsearch(te, &tree_roots[tree_idx], tree_compare_func);
            push((DCLANG_FLT)((*(struct tree_entry **)retval)->value));
            NEXT;
        OP_TREEWALK:
            if (data_stack_ptr < 1) {
                printf("twalk -- stack underflow! needs <tree_root> on the stack.\n");
                return;
            }
            tree_idx = (DCLANG_PTR) POP;
            twalk(tree_roots[tree_idx], print_node);
            NEXT;
        OP_TREEDELETE:
            if (data_stack_ptr < 2) {
                printf("tdel -- stack underflow! needs <tree_root> <key> on the stack.\n");
                return;
            }
            // Pop args
            search_key = (char *)(DCLANG_PTR) POP;
            tree_idx = (DCLANG_PTR) POP;
            te_del = make_tree_entry(dclang_strdup(search_key), 0);
            tdelete(te_del, &tree_roots[tree_idx], tree_compare_func);
            dclang_free(te_del);
            NEXT;
        #ifdef HAS_TREEDESTROY
        OP_TREEDESTROY:
            if (data_stack_ptr < 1) {
                printf("tdestroy -- stack underflow! needs <tree_root> on the stack.\n");
                return;
            }
            tree_idx = (DCLANG_PTR) POP;
            tdestroy(tree_roots[tree_idx], dclang_free);
            tree_roots[tree_idx] = NULL;
            NEXT;
        #endif
        // linked lists
        OP_LISTMAKE:
            // Allocate memory for the head node
            list = (struct Node *)dclang_malloc(sizeof(struct Node));
            // Set the head node to point to itself in both directions
            list->next = list;
            list->prev = list;
            // Push the pointer to the head node onto the stack
            push((DCLANG_PTR)list);
            NEXT;
        OP_LISTNEXT:
            if (data_stack_ptr < 1) {
                printf("_lnext -- stack underflow; needs <existing_list_node> on the stack! ");
                return;
            }
            list_ptr = (DCLANG_PTR)POP;
            // Convert pointers to the actual node structure
            list = (struct Node *)list_ptr;
            next = list->next;
            push((DCLANG_PTR) next);
            NEXT;
        OP_LISTPUSH:
            if (data_stack_ptr < 2) {
                printf("lpush -- stack underflow; needs <list> <value> on the stack! ");
                return;
            }
            // Pop args
            value = POP;
            list_ptr = (DCLANG_PTR)POP;
            // Convert pointers to the actual node structure
            list = (struct Node *)list_ptr;
            // Allocate memory for the new node
            new_node = (struct Node *)dclang_malloc(sizeof(struct Node));
            // Initialize the new node
            new_node->data = value;
            // Insert the new node before the head node (at the tail)
            insque(new_node, list->prev);
            NEXT;
        OP_LISTPOP:
            if (data_stack_ptr < 1) {
                printf("lpop -- stack underflow; needs <list> on the stack! ");
                return;
            }
            // Pop arg
            list_ptr = (DCLANG_PTR)POP;
            // Convert pointers to the actual node structure
            list = (struct Node *)list_ptr;
            // Check if the list is empty
            if (list->next == list) {
                printf("lpop -- list is empty! ");
                return;
            }
            // Get the tail node
            tail_node = list->prev;
            // Remove the tail node from the list
            remque(tail_node);
            // Push the data of the tail node onto the stack
            push((DCLANG_FLT)tail_node->data);
            // Free the memory of the popped node
            dclang_free(tail_node);
            NEXT;
        OP_LISTSET:
            if (data_stack_ptr < 3) {
                printf("l! -- stack underflow; needs <list> <slot> <value> on the stack! ");
                return;
            }
            // Pop args
            value = POP;
            slot = (DCLANG_LONG)POP;
            list_ptr = (DCLANG_PTR)POP;
            // Convert pointers to the actual node structure
            list = (struct Node *)list_ptr;
            // Find the Nth node in the linked list
            node = list;
            for (int i = 0; i < slot + 1; i++) {
                if (node->next == list) {
                    // Reached the end of the list
                    printf("l! -- slot out of bounds! ");
                    return;
                }
                node = node->next;
            }
            node->data = value;
            NEXT;
        OP_LISTGET:
            if (data_stack_ptr < 2) {
                printf("l@ -- stack underflow; needs <list> <slot> on the stack! ");
                return;
            }
            // Pop args
            slot = (DCLANG_LONG)POP;
            list_ptr = (DCLANG_PTR)POP;
            // Convert pointers to the actual node structure
            list = (struct Node *)list_ptr;
            // Find the Nth node in the linked list
            node = list;
            for (int i = 0; i < slot + 1; i++) {
                if (node->next == list) {
                    // Reached the end of the list
                    printf("l@ -- slot out of bounds! ");
                    return;
                }
                node = node->next;
            }
            // Push the data of the node onto the stack
            push((DCLANG_FLT)node->data);
            NEXT;
        OP_LISTINSERT:
            if (data_stack_ptr < 3) {
                printf("lins -- stack underflow; needs <list> <slot> <value> on the stack! ");
                return;
            }
            // Pop args
            value = POP;
            slot = (DCLANG_LONG)POP;
            list_ptr = (DCLANG_PTR)POP;
            // Convert pointers to the actual node structure
            list = (struct Node *)list_ptr;
            // Allocate memory for the new node
            new_node = (struct Node *)dclang_malloc(sizeof(struct Node));
            // Initialize the new node
            new_node->data = value;
            // Find the node before which to insert the new node
            node_before = list;
            for (int i = 0; i < slot; i++) {
                if (node_before->next == list) {
                    // Reached the end of the list
                    printf("lins -- slot out of bounds! ");
                    dclang_free(new_node);
                    return;
                }
                node_before = node_before->next;
            }
            // Insert the new node before the specified node
            insque(new_node, node_before);
            NEXT;
        OP_LISTREMOVE:
            if (data_stack_ptr < 2) {
                printf("lrem -- stack underflow; needs <list> <slot> on the stack! ");
                return;
            }
            // Pop the node slot and list pointer and node slot from the stack
            slot = (DCLANG_LONG)POP;
            list_ptr = (DCLANG_PTR)POP;
            // Convert pointers to the actual node structure
            list = (struct Node *)list_ptr;
            // Find the node to remove
            node_to_remove = list;
            for (int i = 0; i < slot + 1; i++) {
                if (node_to_remove->next == list) {
                    // Reached the end of the list
                    printf("lrem -- slot out of bounds! ");
                    return;
                }
                node_to_remove = node_to_remove->next;
            }
            // Remove the specified node from the list
            remque(node_to_remove);
            // Free the memory of the removed node
            dclang_free(node_to_remove);
            NEXT;
        OP_LISTSIZE:
            if (data_stack_ptr < 1) {
                printf("lsize -- stack underflow; needs <list> on the stack! ");
                return;
            }
            // Pop the list pointer from the stack
            list_ptr = POP;
            // Convert pointer to the actual node structure
            list = (struct Node *)list_ptr;
            // Initialize the size counter
            size = 0;
            // Traverse the linked list and count each node
            node = list->next;
            while (node != list) {
                size++;
                node = node->next;
            }
            // Push the size onto the stack
            push((DCLANG_LONG)size);
            NEXT;
        OP_LISTDELETE:
            if (data_stack_ptr < 1) {
                printf("ldel -- stack underflow; needs <list> on the stack! ");
                return;
            }
            // Pop the list pointer from the stack
            list_ptr = (DCLANG_PTR)POP;
            // Convert pointer to the actual node structure
            list = (struct Node *)list_ptr;
            // Traverse the linked list and free each node in the 'next' direction
            node = list->next;
            while (node != list) {
                next = node->next;
                dclang_free(node);
                node = next;
            }
            // Free the head node
            // Reset the list to a blank head node
            list->next = list;
            list->prev = list;
            NEXT;
        // looping
        OP_TIMES:
            return_stack[return_stack_ptr++] = iptr;
            times_info[times_ptr++] = (DCLANG_LONG) POP;
            loop_counter[loop_counter_ptr++] = 0;
            NEXT;
        OP_EXITTIMES:
            loop_counter[--loop_counter_ptr] = 0;
            --return_stack_ptr;
            --times_ptr;
            NEXT;
        OP_AGAIN:
            if (loop_counter[loop_counter_ptr - 1] < times_info[times_ptr - 1] - 1) {
                loop_counter[loop_counter_ptr - 1] += 1;
                iptr = return_stack[return_stack_ptr - 1];
            } else {
                loop_counter[--loop_counter_ptr] = 0;
                --return_stack_ptr;
                --times_ptr;
            }
            NEXT;
        OP_FOR:
            return_stack[return_stack_ptr++] = iptr;
            fl_stack[fl_ptr].step = (DCLANG_LONG) POP;
            loop_counter[loop_counter_ptr++] = (DCLANG_LONG) POP;
            fl_stack[fl_ptr++].limit = (DCLANG_LONG) POP;
            NEXT;
        OP_EXITFOR:
            --fl_ptr;
            loop_counter[--loop_counter_ptr] = 0;
            --return_stack_ptr;
            NEXT;
        OP_NEXT:
            if (fl_stack[fl_ptr - 1].step > 0) {
                if (loop_counter[loop_counter_ptr - 1] < \
                        (fl_stack[fl_ptr - 1].limit \
                         - fl_stack[fl_ptr - 1].step)) {
                    loop_counter[loop_counter_ptr - 1] += fl_stack[fl_ptr - 1].step;
                    iptr = return_stack[return_stack_ptr - 1];
                } else {
                    --fl_ptr;
                    loop_counter[--loop_counter_ptr] = 0;
                    --return_stack_ptr;
                }
            } else {
                if (loop_counter[loop_counter_ptr - 1] > \
                        (fl_stack[fl_ptr - 1].limit \
                         - fl_stack[fl_ptr - 1].step)) {
                    loop_counter[loop_counter_ptr - 1] += fl_stack[fl_ptr - 1].step;
                    iptr = return_stack[return_stack_ptr - 1];
                } else {
                    --fl_ptr;
                    loop_counter[--loop_counter_ptr] = 0;
                    --return_stack_ptr;
                }
            }
            NEXT;
        OP_I:
            push_no_check(loop_counter[loop_counter_ptr - 1]);
            NEXT;
        OP_J:
            push_no_check(loop_counter[loop_counter_ptr - 2]);
            NEXT;
        OP_K:
            push_no_check(loop_counter[loop_counter_ptr - 3]);
            NEXT;
        // jump if zero (false)
        OP_JUMPZ:
            truth = (DCLANG_LONG) POP;
            if (!truth) {
                iptr = (DCLANG_PTR) prog[iptr].param;
                goto *dispatch_table[prog[iptr].opcode];
            }
            NEXT;
        // unconditional jump
        OP_JUMPU:
            iptr = (DCLANG_PTR) prog[iptr].param;
            goto *dispatch_table[prog[iptr].opcode];
        // if-else-endif
        OP_IF:
            // mark our location
            return_stack[return_stack_ptr++] = iptr;
            // set jump location for 'else'...w/o 'where' location
            // will be filled in by 'else'
            prog[iptr].opcode = OP_JUMPZ;
            prog[iptr++].param = 0;
            return;  // only in def_mode = 1
        OP_ELSE:
            // get the last starting point of the 'if' clause
            DCLANG_LONG if_val = return_stack[--return_stack_ptr];
            // mark out current location on the return stack
            return_stack[return_stack_ptr++] = iptr;
            // set the unconditional jump, but no 'where' yet
            // (will be filled in later by 'endif')....
            prog[iptr].opcode = OP_JUMPU;
            prog[iptr].param = 0;
            // update old if val goto:
            prog[if_val].param = ++iptr;
            return;  // only in def_mode = 1
        OP_ENDIF:
            DCLANG_LONG last_val = return_stack[--return_stack_ptr];
            prog[last_val].param = iptr;
            return;  // only in def_mode = 1
        OP_CALL:
            locals_base_idx += 8;
            // mark where we are for restoration later
            return_stack[return_stack_ptr++] = iptr;
            // set word target; execute word target
            iptr = (DCLANG_PTR) prog[iptr].param;
            goto *dispatch_table[prog[iptr].opcode];
        OP_SET_LOCAL:
            idx = (DCLANG_PTR) prog[iptr].param;
            locals_vals[locals_base_idx + idx] = POP;
            NEXT;
        OP_GET_LOCAL:
            idx = (DCLANG_PTR) prog[iptr].param;
            push(locals_vals[locals_base_idx + idx]);
            NEXT;
        OP_RETURN:
            // restore locals_base_idx
            locals_base_idx -= 8;
            // restore the old iptr
            iptr = return_stack[--return_stack_ptr];
            NEXT;
        // time and date
        OP_CLOCK:
            gettimeofday(&tval, NULL);
            push( ((DCLANG_FLT) tval.tv_sec) + (((DCLANG_FLT) tval.tv_usec) / 1000000) );
            NEXT;
        OP_EPOCH:
            gettimeofday(&tval, NULL);
            push(tval.tv_sec);
            NEXT;
        OP_EPOCH_TO_DT:
            if (data_stack_ptr < 1)
            {
                printf("epoch->dt -- needs <epoch_int> <output_format> on the stack.\n");
                return;
            }
            // input string setup
            fmt = (DCLANG_ULONG) POP;
            if (fmt < MIN_STR || fmt > MAX_STR)
            {
                printf("epoch->dt -- <output_format> string address out-of-range.\n");
                return;
            }
            DCLANG_ULONG in_epoch_uint = (DCLANG_ULONG) POP;
            time_t in_epoch = (time_t) in_epoch_uint;
            char tmbuf[256];
            memset(&tmbuf[0], 0, 256);
            struct tm *loctime = localtime(&in_epoch);
            if (strftime(tmbuf, 256, (char *) fmt, loctime) == 0)
            {
                printf("strftime, a low-level call of 'epoch->dt', returned an error.\n");
                return;
            }
            bufaddr = (DCLANG_PTR) tmbuf;
            if (bufaddr > MAX_STR || MAX_STR == 0)
            {
                MAX_STR = bufaddr;
            }
            if (bufaddr < MIN_STR || MIN_STR == 0)
            {
                MIN_STR = bufaddr;
            }
            push(bufaddr);
            NEXT;
        OP_DT_TO_EPOCH:
            if (data_stack_ptr < 2)
            {
                printf("dt->epoch: need <date> (like \"2020-01-01 12:14:13\") and <input_format> on the stack.\n");
                return;
            }
            // input string setup
            fmt = (DCLANG_ULONG) POP;
            DCLANG_ULONG to_conv = (DCLANG_ULONG) POP;
            if (fmt < MIN_STR || fmt > MAX_STR)
            {
                printf("dt->epoch -- <input_format> string address out-of-range.\n");
                return;
            }
            if (to_conv < MIN_STR || to_conv > MAX_STR)
            {
                printf("dt->epoch -- <date> string address out-of-range.\n");
                return;
            }
            // memset the dt_conv_tm
            struct tm dt_epoch_tm;
            memset(&dt_epoch_tm, 0, sizeof(dt_epoch_tm));
            // convert to broken time
            if (strptime((char *)to_conv, (char *) fmt, &dt_epoch_tm) == NULL)
            {
                printf("Conversion to broken time failed in 'dt->epoch'\n");
                return;
            }
            // do the conversion to seconds since epoch
            time_t res_time = mktime(&dt_epoch_tm);
            push((DCLANG_ULONG) res_time);
            NEXT;
        OP_SLEEP:
            if (data_stack_ptr < 1) {
                printf("sleep -- need a <time_in_secs> on the stack!\n");
                return;
            }
            DCLANG_FLT sleeptime = POP;
            struct timespec t1, t2;
            t1.tv_sec = floor(sleeptime);
            t1.tv_nsec = round(fmod(sleeptime, 1) * 1000000000);
            nanosleep(&t1, &t2);
            NEXT;
        OP_CR:
            fprintf(ofp, "\n");
            NEXT;
        OP_PRINT:
            if (data_stack_ptr < 1)
            {
                printf("print -- stack underflow; need a valid <str_pointer> on the stack!\n");
                return;
            }
            str_ptr_addr = (DCLANG_PTR) POP;
            if (str_ptr_addr == 0)
            {
                printf("print -- Nothing to print.\n");
                return;
            }
            if (str_ptr_addr < MIN_STR || str_ptr_addr > MAX_STR)
            {
                perror("print -- String address out-of-range.\n");
                return;
            }
            fprintf(ofp, "%s", (char *) str_ptr_addr);
            fflush(ofp);
            NEXT;
        OP_EMIT:
            if (data_stack_ptr < 1)
            {
                printf("emit -- stack underflow; need <char_num> between 0 and 127 on the stack! ");
                return;
            }
            char char_code = (char) POP;
            fprintf(ofp, "%c", char_code);
            fflush(ofp);
            NEXT;
        OP_UEMIT:
            if (data_stack_ptr < 1)
            {
                printf("uemit -- stack underflow; need <char_num> between 0x00 and 0x10FFFF on the stack! ");
                return;
            }
            long unsigned long uchar_code = (long unsigned long) POP;
            long ulen = utf8_encode(utf8_buf, uchar_code);
            fprintf(ofp, "%s", utf8_buf);
            fflush(ofp);
            NEXT;
        OP_BYTES32:
            DCLANG_INT byteval = (DCLANG_INT) POP;
            char low = (char) byteval & 0xff;
            byteval >>= 8;
            char lowmid = (char) byteval & 0xff;
            byteval >>= 8;
            char highmid = (char) byteval & 0xff;
            byteval >>= 8;
            char high = (char) byteval & 0xff;
            fputc(low, ofp);
            fputc(lowmid, ofp);
            fputc(highmid, ofp);
            fputc(high, ofp);
            NEXT;
        OP_ISALNUM:
            if (data_stack_ptr < 1)
            {
                printf("isalnum -- stack underflow; need <string_char> on the stack! ");
                return;
            }
            str_ptr_addr = (DCLANG_PTR) POP;
            if (str_ptr_addr < MIN_STR || str_ptr_addr > MAX_STR)
            {
                perror("isalnum -- String address out-of-range.");
                return;
            }
            char_ptr = (char *) str_ptr_addr;
            if (isalnum(char_ptr[0])) {
                push((DCLANG_LONG) -1);
            } else {
                push((DCLANG_LONG) 0);
            }
            NEXT;
        OP_ISALPHA:
            if (data_stack_ptr < 1)
            {
                printf("isalpha -- stack underflow; need <string_char> on the stack! ");
                return;
            }
            str_ptr_addr = (DCLANG_PTR) POP;
            if (str_ptr_addr < MIN_STR || str_ptr_addr > MAX_STR)
            {
                perror("isalpha -- String address out-of-range.");
                return;
            }
            char_ptr = (char *) str_ptr_addr;
            if (isalpha(char_ptr[0])) {
                push((DCLANG_LONG) -1);
            } else {
                push((DCLANG_LONG) 0);
            }
            NEXT;
        OP_ISCNTRL:
            if (data_stack_ptr < 1)
            {
                printf("iscntrl -- stack underflow; need a <string_char> on the stack! ");
                return;
            }
            str_ptr_addr = (DCLANG_PTR) POP;
            if (str_ptr_addr < MIN_STR || str_ptr_addr > MAX_STR)
            {
                perror("iscntrl -- String address out-of-range.");
                return;
            }
            char_ptr = (char *) str_ptr_addr;
            if (iscntrl(char_ptr[0])) {
                push((DCLANG_LONG) -1);
            } else {
                push((DCLANG_LONG) 0);
            }
            NEXT;
        OP_ISDIGIT:
            if (data_stack_ptr < 1)
            {
                printf("isdigit -- stack underflow; need a <string_char> on the stack! ");
                return;
            }
            str_ptr_addr = (DCLANG_PTR) POP;
            if (str_ptr_addr < MIN_STR || str_ptr_addr > MAX_STR)
            {
                perror("isdigit -- String address out-of-range.");
                return;
            }
            char_ptr = (char *) str_ptr_addr;
            if (isdigit(char_ptr[0])) {
                push((DCLANG_LONG) -1);
            } else {
                push((DCLANG_LONG) 0);
            }
            NEXT;
        OP_ISGRAPH:
            if (data_stack_ptr < 1)
            {
                printf("isgraph -- stack underflow; need a <string_char> on the stack! ");
                return;
            }
            str_ptr_addr = (DCLANG_PTR) POP;
            if (str_ptr_addr < MIN_STR || str_ptr_addr > MAX_STR)
            {
                perror("isgraph -- String address out-of-range.");
                return;
            }
            char_ptr = (char *) str_ptr_addr;
            if (isgraph(char_ptr[0])) {
                push((DCLANG_LONG) -1);
            } else {
                push((DCLANG_LONG) 0);
            }
            NEXT;
        OP_ISLOWER:
            if (data_stack_ptr < 1)
            {
                printf("islower -- stack underflow; need a <string_char> on the stack! ");
                return;
            }
            str_ptr_addr = (DCLANG_PTR) POP;
            if (str_ptr_addr < MIN_STR || str_ptr_addr > MAX_STR)
            {
                perror("islower -- String address out-of-range.");
                return;
            }
            char_ptr = (char *) str_ptr_addr;
            if (islower(char_ptr[0])) {
                push((DCLANG_LONG) -1);
            } else {
                push((DCLANG_LONG) 0);
            }
            NEXT;
        OP_ISPRINT:
            if (data_stack_ptr < 1)
            {
                printf("isprint -- stack underflow; need a <string_char> on the stack! ");
                return;
            }
            str_ptr_addr = (DCLANG_PTR) POP;
            if (str_ptr_addr < MIN_STR || str_ptr_addr > MAX_STR)
            {
                perror("isprint -- String address out-of-range.");
                return;
            }
            char_ptr = (char *) str_ptr_addr;
            if (isprint(char_ptr[0])) {
                push((DCLANG_LONG) -1);
            } else {
                push((DCLANG_LONG) 0);
            }
            NEXT;
        OP_ISPUNCT:
            if (data_stack_ptr < 1)
            {
                printf("ispunct -- stack underflow; need a <string_char> on the stack! ");
                return;
            }
            str_ptr_addr = (DCLANG_PTR) POP;
            if (str_ptr_addr < MIN_STR || str_ptr_addr > MAX_STR)
            {
                perror("ispunct -- String address out-of-range.");
                return;
            }
            char_ptr = (char *) str_ptr_addr;
            if (ispunct(char_ptr[0])) {
                push((DCLANG_LONG) -1);
            } else {
                push((DCLANG_LONG) 0);
            }
            NEXT;
        OP_ISSPACE:
            if (data_stack_ptr < 1)
            {
                printf("isspace -- stack underflow; need a <string_char> on the stack! ");
                return;
            }
            str_ptr_addr = (DCLANG_PTR) POP;
            if (str_ptr_addr < MIN_STR || str_ptr_addr > MAX_STR)
            {
                perror("isspace -- String address out-of-range.");
                return;
            }
            char_ptr = (char *) str_ptr_addr;
            if (isspace(char_ptr[0])) {
                push((DCLANG_LONG) -1);
            } else {
                push((DCLANG_LONG) 0);
            }
            NEXT;
        OP_ISUPPER:
            if (data_stack_ptr < 1)
            {
                printf("isupper -- stack underflow; need a <string_char> on the stack! ");
                return;
            }
            str_ptr_addr = (DCLANG_PTR) POP;
            if (str_ptr_addr < MIN_STR || str_ptr_addr > MAX_STR)
            {
                perror("isupper -- String address out-of-range.");
                return;
            }
            char_ptr = (char *) str_ptr_addr;
            if (isupper(char_ptr[0])) {
                push((DCLANG_LONG) -1);
            } else {
                push((DCLANG_LONG) 0);
            }
            NEXT;
        OP_ISXDIGIT:
            if (data_stack_ptr < 1)
            {
                printf("isxdigit -- stack underflow; need a <string_char> on the stack! ");
                return;
            }
            str_ptr_addr = (DCLANG_PTR) POP;
            if (str_ptr_addr < MIN_STR || str_ptr_addr > MAX_STR)
            {
                perror("isxdigit -- String address out-of-range.");
                return;
            }
            char_ptr = (char *) str_ptr_addr;
            if (isxdigit(char_ptr[0])) {
                push((DCLANG_LONG) -1);
            } else {
                push((DCLANG_LONG) 0);
            }
            NEXT;
        OP_TOHEX:
            if (data_stack_ptr < 1)
            {
                printf("tohex -- stack underflow! Needs a <number> on the stack\n");
                return;
            }
            DCLANG_LONG val = (DCLANG_LONG) POP;
            bufsize = snprintf(NULL, 0, "0x%.2lx", val);
            str = dclang_malloc(bufsize + 1);
            snprintf(str, bufsize + 1, "0x%.2lx", val);
            str_ptr_addr = (DCLANG_PTR) str;
            if (str_ptr_addr < MIN_STR || MIN_STR == 0)
            {
                MIN_STR = str_ptr_addr;
            }
            if (str_ptr_addr + bufsize + 1 > MAX_STR || MAX_STR == 0)
            {
                MAX_STR = str_ptr_addr + bufsize + 1;
            }
            push((DCLANG_PTR) str_ptr_addr);
            NEXT;
        OP_TOSTR:
            if (data_stack_ptr < 1)
            {
                printf("tostr -- needs a <number> on stack!\n");
                return;
            }
            DCLANG_FLT var = POP;
            bufsize = snprintf(NULL, 0, "%g", var);
            str = dclang_malloc(bufsize + 1);
            snprintf(str, bufsize + 1, "%g", var);
            str_ptr_addr = (DCLANG_PTR) str;
            if (str_ptr_addr < MIN_STR || MIN_STR == 0)
            {
                MIN_STR = str_ptr_addr;
            }
            if (str_ptr_addr + bufsize + 1 > MAX_STR || MAX_STR == 0)
            {
                MAX_STR = str_ptr_addr + bufsize + 1;
            }
            push((DCLANG_PTR) str_ptr_addr);
            NEXT;
        OP_TONUM:
            if (data_stack_ptr < 1)
            {
                printf("tonum -- needs a <str-pointer> on stack!\n");
                return;
            }
            str_ptr_addr = (DCLANG_PTR) POP;
            if (str_ptr_addr < MIN_STR || str_ptr_addr > MAX_STR)
            {
                perror("tonum -- String address out-of-range.\n");
                return;
            }
            str = (char *) str_ptr_addr;
            DCLANG_FLT num = strtod(str, NULL);
            push(num);
            NEXT;
        OP_ORD:
            if (data_stack_ptr < 1)
            {
                printf("ord -- stack underflow! Needs a <single-character-in-double-quotes> on the stack.\n");
                return;
            }
            str_ptr_addr = (DCLANG_PTR) POP;
            if (str_ptr_addr < (DCLANG_PTR) &memory_pool)
            {
                perror("ord -- String address out-of-range.\n");
                return;
            }
            char *string_loc = (char *) str_ptr_addr;
            push((int) *string_loc);
            NEXT;
        OP_TOLOWER:
            if (data_stack_ptr < 1)
            {
                printf("tolower -- needs a <source_str> pointer on the stack!\n");
                return;
            }
            str_ptr_addr = (DCLANG_PTR) POP;
            if (str_ptr_addr < MIN_STR || str_ptr_addr > MAX_STR)
            {
                perror("tolower -- String address out-of-range.\n");
                return;
            }
            str = (char *) str_ptr_addr;
            buflen = (DCLANG_PTR) strlen(str);
            buf = (char *) dclang_malloc(buflen);
            str_ptr_addr = (DCLANG_PTR) buf;
            i = 0;
            ch = 0;
            while(*(str + i)) {
              ch = (int) *(str + i);
              memset(buf + i, tolower(ch), 1);
              i++;
            }
            if (str_ptr_addr < MIN_STR || MIN_STR == 0)
            {
                MIN_STR = str_ptr_addr;
            }
            if (str_ptr_addr + buflen > MAX_STR || MAX_STR == 0)
            {
                MAX_STR = str_ptr_addr + buflen;
            }
            push((DCLANG_PTR) buf);
            NEXT;
        OP_TOUPPER:
            if (data_stack_ptr < 1)
            {
                printf("toupper -- needs a <source_str> pointer on the stack!\n");
                return;
            }
            str_ptr_addr = (DCLANG_PTR) POP;
            if (str_ptr_addr < MIN_STR || str_ptr_addr > MAX_STR)
            {
                perror("toupper -- String address out-of-range.\n");
                return;
            }
            str = (char *) str_ptr_addr;
            buflen = (DCLANG_PTR) strlen(str);
            buf = (char *) dclang_malloc(buflen);
            str_ptr_addr = (DCLANG_PTR) buf;
            i = 0;
            ch = 0;
            while(*(str + i)) {
              ch = (int) *(str + i);
              memset(buf + i, toupper(ch), 1);
              i++;
            }
            if (str_ptr_addr < MIN_STR || MIN_STR == 0)
            {
                MIN_STR = str_ptr_addr;
            }
            if (str_ptr_addr + buflen > MAX_STR || MAX_STR == 0)
            {
                MAX_STR = str_ptr_addr + buflen;
            }
            push((DCLANG_PTR) buf);
            NEXT;
        OP_STRLEN:
            if (data_stack_ptr < 1)
            {
                printf("strlen -- stack underflow; needs a <str> on the stack!\n");
                return;
            }
            str_ptr_addr = (DCLANG_PTR) POP;
            if (str_ptr_addr < MIN_STR || str_ptr_addr > MAX_STR)
            {
                perror("strlen -- String address out-of-range.\n");
                return;
            }
            str = (char *) str_ptr_addr;
            push((DCLANG_PTR) strlen(str));
            NEXT;
        OP_STREQ:
            if (data_stack_ptr < 2)
            {
                printf("str= -- stack underflow; need <str> <str> on the stack!\n");
                return;
            }
            str_ptr_addr2 = (DCLANG_PTR) POP;
            str_ptr_addr = (DCLANG_PTR) POP;
            if (str_ptr_addr < MIN_STR || str_ptr_addr > MAX_STR)
            {
                perror("strlen -- First given string address out-of-range.\n");
                return;
            }
            if (str_ptr_addr2 < MIN_STR || str_ptr_addr2 > MAX_STR)
            {
                perror("strlen -- Second given string address out-of-range.\n");
                return;
            }
            str1 = (char *) str_ptr_addr;
            str2 = (char *) str_ptr_addr2;
            push(((DCLANG_LONG) strcmp(str1, str2) == 0) * -1);
            NEXT;
        OP_STRLT:
            if (data_stack_ptr < 2)
            {
                printf("str< -- stack underflow; need <str> <str> on the stack!\n");
                return;
            }
            str_ptr_addr2 = (DCLANG_PTR) POP;
            str_ptr_addr = (DCLANG_PTR) POP;
            if (str_ptr_addr < MIN_STR || str_ptr_addr > MAX_STR)
            {
                perror("str< -- First given string address out-of-range.\n");
                return;
            }
            if (str_ptr_addr2 < MIN_STR || str_ptr_addr2 > MAX_STR)
            {
                perror("str< -- Second given string address out-of-range\n");
                return;
            }
            str1 = (char *) str_ptr_addr;
            str2 = (char *) str_ptr_addr2;
            push(((DCLANG_LONG) strcmp(str1, str2) < 0) * -1);
            NEXT;
        OP_STRGT:
            if (data_stack_ptr < 2)
            {
                printf("str> -- stack underflow; need <str> <str> on the stack!\n");
                return;
            }
            str_ptr_addr2 = (DCLANG_PTR) POP;
            str_ptr_addr = (DCLANG_PTR) POP;
            if (str_ptr_addr < MIN_STR || str_ptr_addr > MAX_STR)
            {
                perror("str> -- First given string address out-of-range\n");
                return;
            }
            if (str_ptr_addr2 < MIN_STR || str_ptr_addr2 > MAX_STR)
            {
                perror("str> -- Second given string address out-of-range.\n");
                return;
            }
            str1 = (char *) str_ptr_addr;
            str2 = (char *) str_ptr_addr2;
            push(((DCLANG_LONG) strcmp(str1, str2) > 0) * -1);
            NEXT;
        OP_STRFIND:
            if (data_stack_ptr < 2)
            {
                printf("strfind -- needs <haystack> <needle> string pointers on stack!\n");
                return;
            }
            str_ptr_addr2 = (DCLANG_PTR) POP;
            str_ptr_addr = (DCLANG_PTR) POP;
            if (str_ptr_addr < MIN_STR || str_ptr_addr > MAX_STR)
            {
                perror("strfind -- 'haystack' (first) string address out-of-range.\n");
                return;
            }
            if (str_ptr_addr2 < MIN_STR || str_ptr_addr2 > MAX_STR)
            {
                perror("strfind -- 'needle' (second) string address out-of-range.\n");
                return;
            }
            str1 = (char *) str_ptr_addr;
            str2 = (char *) str_ptr_addr2;
            push((DCLANG_LONG) strstr(str1, str2));
            NEXT;
        OP_STRSPN:
            if (data_stack_ptr < 2)
            {
                printf("strspn -- needs <str> <test_chars_str> pointers on the stack!\n");
                return;
            }
            delim = (DCLANG_PTR) POP;
            strpt = (DCLANG_PTR) POP;
            if ((delim != 0) && (delim < MIN_STR || delim > MAX_STR))
            {
                perror("strspn -- <delim> string address out-of-range.\n");
                return;
            }
            if (strpt < MIN_STR || strpt > MAX_STR)
            {
                perror("strspn -- <str> string address out-of-range.\n");
                return;
            }
            push((DCLANG_LONG) strspn((char *)strpt, (char *)delim));
            NEXT;
        OP_STRCSPN:
            if (data_stack_ptr < 2)
            {
                printf("strcspn -- needs <str> <test_chars_str> pointers on the stack!\n");
                return;
            }
            delim = (DCLANG_PTR) POP;
            strpt = (DCLANG_PTR) POP;
            if ((delim != 0) && (delim < MIN_STR || delim > MAX_STR))
            {
                perror("strcspn -- <test_chars_str> string address out-of-range.\n");
                return;
            }
            if (strpt < MIN_STR || strpt > MAX_STR)
            {
                perror("strcspn -- <str> string address out-of-range.");
                return;
            }
            push((DCLANG_LONG) strcspn((char *)strpt, (char *)delim));
            NEXT;
        OP_STRTOK:
            if (data_stack_ptr < 3)
            {
                printf("strtok -- needs <str> <delim> <saveptr> string pointers on stack!\n");
                printf("<saveptr> should be a variable slot declared with `var`, without being dereferenced with `@`.\n");
                printf("e.g.:\n\nvar mysave\n\"split.this.string!\" \".\" mysave strtok print cr\n");
                printf("split\nnull \".\" mysave strtok print cr\n");
                printf("this\nnull \".\" mysave strtok print cr\n");
                printf("string!\n\n");
                return;
            }
            DCLANG_PTR savepoint = (DCLANG_PTR) POP;
            char **savepoint_ptr = (char **) &vars[savepoint];
            delim = (DCLANG_PTR) POP;
            strpt = (DCLANG_PTR) POP;
            if ((strpt != 0) && (strpt < MIN_STR || strpt > MAX_STR))
            {
                perror("strtok -- <str> (first) string address out-of-range.\n");
                return;
            }
            if (delim < MIN_STR || delim > MAX_STR)
            {
                perror("strtok -- <delim> (second) string address out-of-range.\n");
                return;
            }
            push((DCLANG_LONG) strtok_r((char *)strpt, (char *)delim, savepoint_ptr));
            NEXT;
        OP_MEMPCPY:
            if (data_stack_ptr < 3)
            {
                printf("mempcpy -- needs <dest> <source> <size> on stack!\n");
                return;
            }
            size = (DCLANG_PTR) POP;
            DCLANG_PTR source = (DCLANG_PTR) POP;
            dest = (DCLANG_PTR) POP;
            if ((dest != 0) && (dest < MIN_STR || dest > MAX_STR))
            {
                perror("mempcpy -- <dest> string address out-of-range.\n");
                return;
            }
            if (source < MIN_STR || source > MAX_STR)
            {
                perror("mempcpy -- <source> string address out-of-range.\n");
                return;
            }
            push(
                (DCLANG_PTR) memcpy(
                (char *)dest, (char *)source, (DCLANG_PTR) size) + size
            );
            NEXT;
        OP_MEMSET:
            if (data_stack_ptr < 3)
            {
                printf("memset -- needs <dest_str> <char-int> <times-int> on stack!\n");
                return;
            }
            DCLANG_PTR times = (DCLANG_PTR) POP;
            DCLANG_PTR chr = (DCLANG_PTR) POP;
            dest = (DCLANG_PTR) POP;
            if ((dest != 0) && (dest < MIN_STR || dest > MAX_STR))
            {
                perror("memset -- <dest> string address out-of-range.\n");
                return;
            }
            push((DCLANG_PTR)memset((char *)dest, (int)chr, (int)times));
            NEXT;
        OP_MKBUF:
            if (data_stack_ptr < 1)
            {
                printf("Stack_underflow!\n");
                printf("mkbuf needs <size-as-integer> on the stack\n");
            }
            size = (DCLANG_PTR) POP;
            buf = (char *) dclang_malloc(size);
            memset(buf, 0, size);
            DCLANG_PTR advance = (DCLANG_PTR) strlen(buf);
            bufaddr = (DCLANG_PTR) buf;
            bufaddr += advance;
            // update print safety:
            if (bufaddr < MIN_STR || MIN_STR == 0)
            {
                MIN_STR = bufaddr;
            }
            if (bufaddr + size + 1 > MAX_STR || MAX_STR == 0)
            {
                MAX_STR = bufaddr + size + 1;
            }
            // done updating
            push(bufaddr);
            NEXT;
        OP_FREE:
            if (data_stack_ptr < 1)
            {
                printf("free -- stack underflow! N.B. This word is actually a no-op, kept for backwards compatibility\n");
                return;
            }
            DCLANG_PTR loc_PTR = (DCLANG_PTR) POP;
            dclang_free((char *) loc_PTR);
            NEXT;
        OP_MEMUSED:
            DCLANG_FLT memused = (DCLANG_FLT) (((float) unused_mem_idx) / ((float) MEMSIZE));
            push(memused);
            NEXT;
        OP_REGCOMP:
            if (data_stack_ptr < 2)
            {
                printf("regcomp -- stack underflow: need <pattern> <regex_flags> on the stack!\n");
                return;
            }
            // Pop the regex pattern from the stack
            regex_flags = (DCLANG_INT) POP;
            DCLANG_PTR pattern_ptr_addr = (DCLANG_PTR)POP;
            if (pattern_ptr_addr < MIN_STR || pattern_ptr_addr > MAX_STR)
            {
                perror("regcomp -- Pattern address out-of-range.\n");
                return;
            }
            char *pattern = (char *)pattern_ptr_addr;
            // Compile the regex pattern
            regex= (regex_t *)dclang_malloc(sizeof(regex_t));
            if (regcomp(regex, pattern, (DCLANG_INT)regex_flags) != 0)
            {
                perror("regcomp -- Error compiling regex pattern\n");
                dclang_free(regex);
                return;
            }
            // Push the compiled regex object pointer onto the stack
            push((DCLANG_PTR)regex);
            NEXT;
        OP_REGEXEC:
            if (data_stack_ptr < 3)
            {
                printf("regexec -- stack underflow; need <regexobj> <string_to_search> <regex_flags> on the stack!\n");
                return;
            }
            // Pop the input string and the compiled regex object from the stack
            regex_flags = (DCLANG_INT) POP;
            DCLANG_PTR input_str_ptr_addr = (DCLANG_PTR)POP;
            DCLANG_PTR regex_obj_PTR = (DCLANG_PTR)POP;
            if (regex_obj_PTR < 0 || input_str_ptr_addr < MIN_STR || input_str_ptr_addr > MAX_STR)
            {
                perror("regexec -- Invalid regex object or string address.\n");
                return;
            }
            regex= (regex_t *)regex_obj_PTR;
            char *input_str = (char *)input_str_ptr_addr;
            // Execute the regex matching
            regmatch_t *match = (regmatch_t *)dclang_malloc(10 * sizeof(regmatch_t));
            if (regexec(regex, input_str, 10, match, (DCLANG_INT)regex_flags) == 0)
            {
                // If a match is found, push the match object onto the stack
                push((DCLANG_PTR)match);
            }
            else
            {
                // No match found, push -1 to indicate failure
                push((DCLANG_LONG)-1);
            }
            NEXT;
        OP_REGREAD:
            if (data_stack_ptr < 2)
            {
                printf("regread -- stack underflow; need <regexec_result> <match_index> on the stack! \n");
                printf("regread will return a start and end index relative to the original searched string. \n");
                printf("If the user actually wants that substring, it makes sense to have saved the original \n");
                printf("string, and put the results of regread at the top of stack, then call `strslice`,\n");
                printf("which needs to be imported from the 'string' module.\n");
                return;
            }
            DCLANG_LONG index = (DCLANG_LONG)POP;
            DCLANG_PTR regmatch_pnt = (DCLANG_PTR)POP;
            if ((DCLANG_LONG)regmatch_pnt > 0) {
                regmatch_t *match = (regmatch_t *)regmatch_pnt;
                push((DCLANG_LONG)(match[index].rm_so));
                push((DCLANG_LONG)(match[index].rm_eo));
            } else {
                push((DCLANG_LONG)-1);
                push((DCLANG_LONG)-1);
            }
            NEXT;
        OP_IMPORT:
            if (data_stack_ptr < 1) {
                printf("import -- stack underflow! ");
            }
            char *importfile = (char *)(unsigned long) POP;
            dclang_import(importfile);
            NEXT;
        OP_FILEOPEN:
            if (data_stack_ptr < 2)
            {
                printf("Stack underflow!\n");
                printf("fopen needs <filename> <open-mode> on the stack\n");
                return;
            }
            // file mode string
            mode = (char *)(DCLANG_PTR) POP;
            // file path
            path = (char *)(DCLANG_PTR) POP;
            // if mode is read or append, file must exist:
            if ( (access(path, F_OK) == -1)
                 && ( !strcmp("r", mode) || !strcmp("r+", mode) ) )
            {
                printf("The file named %s doesn't appear to exist, " \
                       "or cannot be accessed.\n", path);
                return;
            }
            file = fopen(path, mode);
            push((DCLANG_PTR)file);
            NEXT;
        OP_FILEMEMOPEN:
            if (data_stack_ptr < 3)
            {
                printf("Stack underflow!\n");
                printf("fmemopen needs <buf (can be 0)> <size> <open-mode> on the stack\n");
                return;
            }
            mode = (char *)(DCLANG_PTR) POP;
            size = (DCLANG_PTR) POP;
            bufaddr = (DCLANG_PTR) POP;
            file = fmemopen((void *)bufaddr, size, mode);
            push((DCLANG_PTR)file);
            NEXT;
        OP_FILECLOSE:
            if (data_stack_ptr < 1)
            {
                printf("Stack underflow!\n");
                printf("fclose needs <file_pointer> on the stack\n");
                return;
            }
            file = (FILE *)(DCLANG_PTR) POP;
            fclose(file);
            NEXT;
        OP_FILEREAD:
            if (data_stack_ptr < 3)
            {
                printf("Stack underflow!\n");
                printf("fread needs <buf_pointer> <number-of-bytes> <file_pointer> on the stack\n");
                return;
            }
            file = (FILE *)(DCLANG_PTR) POP;
            DCLANG_PTR num_bytes_in = (DCLANG_PTR) POP;
            buf = (char *)(DCLANG_PTR) POP;
            num_bytes = fread(buf, 1, num_bytes_in, file);
            // update print safety:
            if ((DCLANG_PTR)buf < MIN_STR || MIN_STR == 0)
            {
                MIN_STR = (DCLANG_PTR)buf;
            }
            if ((DCLANG_PTR)buf + num_bytes + 1 > MAX_STR || MAX_STR == 0)
            {
                MAX_STR = (DCLANG_PTR)buf + num_bytes + 1;
            }
            // push the number of bytes read
            push((DCLANG_LONG) num_bytes);
            NEXT;
        OP_FILEREADLINE:
            if (data_stack_ptr < 1)
            {
                printf("Stack underflow!\n");
                printf("freadline needs <file_pointer> on the stack\n");
                return;
            }
            file = (FILE *)(DCLANG_PTR) POP;
            num_bytes = getline(&linebuf, &linelen, file);
            // update print safety:
            if ((DCLANG_PTR) linebuf < MIN_STR || MIN_STR == 0)
            {
                MIN_STR = (DCLANG_PTR) linebuf;
            }
            if ((DCLANG_PTR) linebuf + num_bytes + 1 > MAX_STR || MAX_STR == 0)
            {
                MAX_STR = (DCLANG_PTR) linebuf + num_bytes + 1;
            }
            // push the address of our new string and length
            push((DCLANG_PTR) linebuf);
            // push the number of bytes read
            push((DCLANG_LONG) num_bytes);
            NEXT;
        OP_FILEREADALL:
            if (data_stack_ptr < 1)
            {
                printf("Stack underflow!\n");
                printf("freadall needs <file_pointer> on the stack\n");
                return;
            }
            DCLANG_PTR chr_cnt = 0;
            DCLANG_PTR req_bufsize = 64;
            DCLANG_LONG ch;
            char *allbuf = (char *) dclang_malloc(req_bufsize);
            memset(allbuf, 0, req_bufsize);
            file = (FILE *)(DCLANG_PTR) POP;
            while ((ch = fgetc(file)) != EOF)
            {
                chr_cnt += 1;
                if (chr_cnt > req_bufsize)
                {
                    req_bufsize *= 2;
                    allbuf = (char *) dclang_realloc(allbuf, req_bufsize);
                }
                memset(allbuf + chr_cnt - 1, ch, 1);
            }
            memset(allbuf + chr_cnt, 0, 1);
            // update print safety:
            if ((DCLANG_PTR) allbuf < MIN_STR || MIN_STR == 0)
            {
                MIN_STR = (DCLANG_PTR) allbuf;
            }
            if ((DCLANG_PTR) allbuf + chr_cnt + 1 > MAX_STR || MAX_STR == 0)
            {
                MAX_STR = (DCLANG_PTR) allbuf + chr_cnt + 1;
            }
            // push the address of our new string and length
            push((DCLANG_PTR) allbuf);
            // push the number of bytes read
            push((DCLANG_LONG) chr_cnt);
            NEXT;
        OP_FILESEEK:
            if (data_stack_ptr < 3)
            {
                printf("Stack underflow!\n");
                printf("fseek needs <file_pointer> <offset> <whence> on the stack\n");
                printf("Whence must be 0 (SEEK_SET), 1 (SEEK_CUR), or 2 (SEEK_END).\n");
                return;
            }
            DCLANG_PTR whence = (DCLANG_PTR) POP;
            if (!(whence >= 0 && whence <= 2))
            {
                printf("Whence parameter must be between 0 and 2 inclusive!\n");
                return;
            }
            DCLANG_LONG offset = (DCLANG_LONG) POP;
            file = (FILE *)(DCLANG_PTR) POP;
            fseek(file, offset, whence);
            NEXT;
        OP_FILETELL:
            if (data_stack_ptr < 1)
            {
                printf("Stack underflow!\n");
                printf("ftell needs a <file_pointer> on the stack\n");
                return;
            }
            file = (FILE *)(DCLANG_PTR) POP;
            DCLANG_PTR mylen = ftell(file);
            push((DCLANG_PTR) mylen);
            NEXT;
        OP_FILEWRITE:
            if (data_stack_ptr < 3)
            {
                printf("fwrite -- needs <string-address> <num_of_bytes> <file_pointer> on the stack\n");
                return;
            }
            file = (FILE *)(DCLANG_PTR) POP;
            num_bytes = (DCLANG_PTR) POP;
            str = (char *)(DCLANG_PTR) POP;
            result = fwrite(str, 1, num_bytes, file);
            push(result);
            NEXT;
        OP_FILEFLUSH:
            if (data_stack_ptr < 1)
            {
                printf("fflush -- needs <file_pointer> on the stack\n");
                return;
            }
            file = (FILE *)(DCLANG_PTR) POP;
            fflush(file);
            NEXT;
        OP_REDIRECT:
            if (data_stack_ptr < 1) {
                printf("redirect -- stack underflow; needs an <output-file-pointer> on the stack\n");
                return;
            }
            ofp = (FILE *)(DCLANG_PTR) POP;
            NEXT;
        OP_RESETOUT:
            ofp = stdout;
            NEXT;
        OP_FLUSHOUT:
            fflush(ofp);
            NEXT;
        OP_OPEN:
            if (data_stack_ptr < 2)
            {
                printf("Stack_underflow!\n");
                printf("open -- needs <filestr> <flagint> on the stack\n");
                return;
            }
            DCLANG_PTR flagint = (DCLANG_PTR) POP;
            path = (char *)(DCLANG_PTR)POP;
            fd = open(path, flagint);
            push((DCLANG_PTR) fd);
            NEXT;
        OP_READ:
            if (data_stack_ptr < 1)
            {
                printf("Stack_underflow!\n");
                printf("read -- needs <file_pointer> <buffer-pointer> <num_bytes> on the stack\n");
                return;
            }
            num_bytes = (DCLANG_PTR) POP;
            buf = (void *)(DCLANG_PTR)POP;
            fd = (int) POP;
            result = read(fd, buf, num_bytes);
            push(result);
            NEXT;
        OP_WRITE:
            if (data_stack_ptr < 3)
            {
                printf("Stack_underflow!\n");
                printf("write needs <file_pointer> <buffer-pointer> <num_bytes> on the stack\n");
                return;
            }
            num_bytes = (DCLANG_PTR) POP;
            buf = (void *)(DCLANG_PTR)POP;
            fd = (int) POP;
            result = write(fd, buf, num_bytes);
            push(result);
            NEXT;
        OP_CLOSE:
            if (data_stack_ptr < 1)
            {
                printf("Stack_underflow!\n");
                printf("close -- needs <file_pointer> on the stack\n");
                return;
            }
            int fp = (int) POP;
            result = close(fp);
            push(result);
            NEXT;
        OP_TCPLISTEN:
            // Sets up a new listening TCP socket 'object' for listening
            // and returns its address onto the stack
            if (data_stack_ptr < 1) {
                printf("tcplisten -- need <port_number> on the\n");
                return;
            }
            sockfd = socket(AF_INET, SOCK_STREAM, 0);
            const int enable = 1;
            setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(DCLANG_INT));
            if (sockfd < 0) {
               perror("tcplisten -- ERROR opening socket.");
            }
            bzero((char *) &serv_addr, sizeof(serv_addr));
            portno = (DCLANG_INT) POP;
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_addr.s_addr = INADDR_ANY;
            serv_addr.sin_port = htons(portno);
            if (bind(sockfd, (struct sockaddr *) &serv_addr,
                  sizeof(serv_addr)) < 0)
                  perror("ERROR on binding");
            listen(sockfd, 5);
            push((DCLANG_INT) sockfd);
            NEXT;
        OP_TCPACCEPT:
            // Take a given tcplisten-ready socket object and actually make it
            // accept a connection. Returns a handle to the established connection.
            // Can be used inside a loop as a basis for a "multi-connection handling"
            // forking server via `fork`.
            if (data_stack_ptr < 1) {
                printf("tcpaccept -- need <socket> on the stack");
                return;
            }
            DCLANG_UINT clilen = sizeof(cli_addr);
            sockfd = (DCLANG_INT) POP;
            DCLANG_INT newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
            if (newsockfd < 0) perror("tcpaccept -- ERROR on accept step!");
            push((DCLANG_INT) newsockfd);
            NEXT;
        OP_TCPCONNECT:
            // A tcp client word. This can reach out to a pre-established tcp server
            // already set up with the above words.
            if (data_stack_ptr < 2) {
                printf("tcpconnect -- need <host> <port> on the stack");
                return;
            }
            sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd < 0) perror("tcpconnect -- ERROR opening socket");
            portno = (DCLANG_INT) POP;
            struct sockaddr_in host_addr;
            host = (char *) (DCLANG_PTR) POP;
            server = gethostbyname(host);
            if (server == NULL) {
                fprintf(stderr, "tcpconnect -- ERROR, no such host\n");
                exit(0);
            }
            bzero((char *) &host_addr, sizeof(host_addr));
            host_addr.sin_family = AF_INET;
            bcopy((char *)server->h_addr, (char *)&host_addr.sin_addr.s_addr, server->h_length);
            host_addr.sin_port = htons(portno);
            if (connect(sockfd, (struct sockaddr *)&host_addr, sizeof(host_addr)) < 0)
                perror("tcpconnect -- ERROR connecting");
            push((DCLANG_INT) sockfd);
            NEXT;
        OP_UDPRECV:
            // Receive data over UDP
            if (data_stack_ptr < 3) {
                printf("udprecv -- need <port_number> <max_bytes> <buffer> on the stack\n");
                return;
            }
            // stack values
            buf = (char *) (DCLANG_PTR) POP;
            DCLANG_INT max_bytes = (DCLANG_INT) POP;
            portno = (DCLANG_INT) POP;
            // make a socket
            sockfd = socket(AF_INET, SOCK_DGRAM, 0);
            if (sockfd < 0) {
                perror("udprecv -- ERROR opening socket");
                return;
            }
            socklen_t udp_clilen = sizeof(udp_cli_addr);
            bzero((char *) &udp_serv_addr, sizeof(udp_serv_addr));
            udp_serv_addr.sin_family = AF_INET;
            udp_serv_addr.sin_addr.s_addr = INADDR_ANY;
            udp_serv_addr.sin_port = htons(portno);
            // bind the socket
            if (bind(sockfd, (struct sockaddr *) &udp_serv_addr,
                     sizeof(udp_serv_addr)) < 0) {
                perror("udprecv -- ERROR on binding");
            }
            result = recvfrom(
                sockfd, buf, max_bytes, 0,
                (struct sockaddr *)&udp_cli_addr, &udp_clilen
            );
            if (result < 0) {
                perror("udprecv -- ERROR receiving data");
            }
            buf[result] = '\0'; // Null terminate the received data
            close(sockfd);
            push((DCLANG_LONG) result);
            NEXT;
        OP_UDPSEND:
            // Send data over UDP to a specified host and port
            if (data_stack_ptr < 3) {
                printf("udpsend -- need <host> <port> <buffer> on the stack\n");
                return;
            }
            // stack values
            buf = (char *) (DCLANG_PTR) POP;
            portno = (DCLANG_INT) POP;
            host = (char *) (DCLANG_PTR) POP;
            // make a socket
            sockfd = socket(AF_INET, SOCK_DGRAM, 0);
            if (sockfd < 0) {
                perror("udpsend -- ERROR opening socket");
                return;
            }
            server = gethostbyname(host);
            if (server == NULL) {
                fprintf(stderr, "udpsend -- ERROR, no such host\n");
                return;
            }
            bzero((char *) &dest_addr, sizeof(dest_addr));
            dest_addr.sin_family = AF_INET;
            bcopy(
                (char *)server->h_addr,
                (char *)&dest_addr.sin_addr.s_addr,
                server->h_length
            );
            dest_addr.sin_port = htons(portno);
            result = sendto(
                sockfd, buf, strlen(buf) + 1, 0,
                (struct sockaddr *)&dest_addr, sizeof(dest_addr)
            );
            if (result < 0) {
                perror("udpsend -- ERROR sending data");
            }
            close(sockfd);
            push((DCLANG_INT) result);
            NEXT;
        OP_BLOCKSIGINT:
            sigemptyset(&block_sigint);
            sigaddset(&block_sigint, SIGINT);
            sigprocmask(SIG_BLOCK, &block_sigint, NULL);
            NEXT;
        OP_UNBLOCKSIGINT:
            sigprocmask(SIG_UNBLOCK, &block_sigint, NULL);
            NEXT;
        OP_FORK:
            // This wrod mainly exists so that a multi-client capable tcp/web server
            // can be had. It is assumed that the return value will be caught, inspected,
            // and handled by the caller, so this is really quite a simple c->dclang mapping.
            // TODO: a future enhancement might be to have a counting system in place for
            // avoiding fork-bomb DoS attacks. So, the introspection of a connection limit
            // before granting a new `fork`.
            push((DCLANG_INT) fork());
            NEXT;
        OP_EXIT:
            if (data_stack_ptr < 1) {
                printf("exit -- stack underflow; need an <integer_exit_code> on the stack");
                return;
            }
            DCLANG_INT code = (DCLANG_INT) POP;
            exit(code);
            NEXT;
        OP_SHOWPRIMITIVES:
            show_primitives();
            NEXT;
        OP_SHOWWORDS:
            for (int x=0; x < num_user_words; x++) {
                printf("Word %i: %s @ %li\n", x, user_words[x].name,\
                                                 user_words[x].word_start);
            }
            NEXT;
        OP_SHOWCONSTS:
            for (int x=0; x < const_idx; x++) {
                printf("Constant %i: %s ==> %.19g\n", x, const_keys[x], const_vals[x]);
            }
            NEXT;
        OP_SHOWVARS:
            for (int x=0; x < var_map_idx; x++) {
                printf("Variable %i: %s @ %li\n", x, var_keys[x], var_vals[x]);
            }
            NEXT;
        OP_EXEC:
            execfunc();
            NEXT;
        OP_INPUT:
            setinput(stdin);
            char input_ch;
            // get a starting marker for length
            unsigned long string_start = input_here;
            // bypass leading whitespace
            do {
                if((input_ch = fgetc(ifp)) == EOF) {
                    exit(0);
                }
            } while(isspace(input_ch));
            while (! strchr("\n", input_ch)) {
                if (strchr("\\", input_ch)) {
                    // consume an extra char due to backslash
                    if ((input_ch = fgetc(ifp)) == EOF) exit(0);
                    // backspace
                    if (strchr("b", input_ch)) {
                        input_ch = 8;
                    }
                    if (strchr("n", input_ch)) {
                        input_ch = 10;
                    }
                    if (strchr("t", input_ch)) {
                        input_ch = 9;
                    }
                }
                input_pad[input_here++] = input_ch;
                if ((input_ch = getchar()) == EOF) exit(0);
            }
            DCLANG_PTR string_addr = (DCLANG_PTR) string_start;
            DCLANG_PTR string_size = (DCLANG_PTR)(input_here - string_start);
            char *string_dest = dclang_malloc(string_size + 1);
            char nullstr[] = "\0";
            memcpy(string_dest, (char *)((DCLANG_PTR)&input_pad[0] + string_addr), string_size);
            str_ptr_addr = (DCLANG_PTR) string_dest;
            if (str_ptr_addr < MIN_STR || MIN_STR == 0) {
                MIN_STR = str_ptr_addr;
            }
            if (str_ptr_addr + string_size + 1 > MAX_STR || MAX_STR == 0) {
                MAX_STR = str_ptr_addr + string_size + 1;
            }
            push(str_ptr_addr);
            revertinput();
            NEXT;
        _OP_SQLITEOPEN:
            const char* db_path = (const char*)(DCLANG_PTR)POP;
            rc = sqlite3_open(db_path, &db);
            if (rc != SQLITE_OK) {
                printf("SQLite3 error: %s\n", sqlite3_errmsg(db));
                return;
            }
            push((DCLANG_PTR)db);
            NEXT;
        _OP_SQLITEPREPARE:
            sql = (char *)(DCLANG_PTR)POP;
            db = (sqlite3 *)(DCLANG_PTR)POP;
            const char *sql_const = (const char *) sql;
            rc = sqlite3_prepare_v2(db, sql_const, -1, &stmt, NULL);
            if (rc != SQLITE_OK) {
                printf("SQLite3 error: %s\n", sqlite3_errmsg(db));
                return;
            }
            push((DCLANG_PTR)stmt);
            NEXT;
        _OP_SQLITESTEP:
            stmt = (sqlite3_stmt *)(DCLANG_PTR)POP;
            rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
                printf("SQLite3 error: %s\n", sqlite3_errmsg(sqlite3_db_handle(stmt)));
                return;
            }
            push((DCLANG_PTR)rc);
            NEXT;
        _OP_SQLITECOLUMN:
            int col_index = (int)POP;
            stmt = (sqlite3_stmt *)(DCLANG_PTR)POP;
            // Get the number of columns in the result set
            int col_count = sqlite3_column_count(stmt);
            // Check if the column index is within a valid range
            if (col_index < 0 || col_index >= col_count) {
                printf("Column index out of bounds! ");
                return;
            }
            char *text = sqlite3_column_text(stmt, col_index);
            DCLANG_PTR text_ptr = (DCLANG_PTR) text;
            int charsize = strlen(text);
            // update print safety:
            if (text_ptr < MIN_STR || MIN_STR == 0)
            {
                MIN_STR = text_ptr;
            }
            if (text_ptr + charsize + 1 > MAX_STR || MAX_STR == 0)
            {
                MAX_STR = text_ptr + charsize + 1;
            }
            push(text_ptr);
            NEXT;
        _OP_SQLITEFINALIZE:
            stmt = (sqlite3_stmt *)(DCLANG_PTR)POP;
            rc = sqlite3_finalize(stmt);
            if (rc != SQLITE_OK) {
                printf("SQLite3 error: %s\n", sqlite3_errmsg(sqlite3_db_handle(stmt)));
                return;
            }
            NEXT;
        _OP_SQLITEEXEC:
            char *zErrMsg = 0;
            sql = (char *)(DCLANG_PTR)POP;
            db = (sqlite3 *)(DCLANG_PTR)POP;
            rc = sqlite3_exec(db, sql, __sqlcallback, 0, &zErrMsg);
            if(rc != SQLITE_OK) {
              fprintf(stderr, "SQL error: %s\n", zErrMsg);
              sqlite3_free(zErrMsg);
            }
            NEXT;
        _OP_SQLITECLOSE:
            db = (sqlite3 *)(DCLANG_PTR)POP;
            rc = sqlite3_close(db);
            if (rc != SQLITE_OK) {
                printf("SQLite3 error: %s\n", sqlite3_errmsg(db));
                return;
            }
            NEXT;
        _OP_PM_LIST:
            // list midi_device information
            int default_in = Pm_GetDefaultInputDeviceID();
            int default_out = Pm_GetDefaultOutputDeviceID();
            for (int i = 0; i < Pm_CountDevices(); i++) {
                char *deflt;
                const PmDeviceInfo *info = Pm_GetDeviceInfo(i);
                printf("%d: %s, %s", i, info->interf, info->name);
                if (info->input) {
                    deflt = (i == default_in ? "default " : "");
                    printf(" (%sinput)", deflt);
                }
                if (info->output) {
                    deflt = (i == default_out ? "default " : "");
                    printf(" (%soutput)", deflt);
                }
                printf("\n");
            }
            NEXT;
        _OP_PM_OPENOUT:
            if (data_stack_ptr < 1)
            {
                printf("_pm_open_out needs a midi_device number on the stack!\n");
                return;
            }
            DCLANG_LONG midi_device = (DCLANG_LONG) POP;
            Pm_OpenOutput(&midi_stream, midi_device, NULL, 0, NULL, NULL , 0);
            NEXT;
        _OP_PM_WS:
            if (data_stack_ptr < 3)
            {
                printf("_pm_ws needs 3 integers on the stack:\n");
                printf("    <status> <data1> <data2>\n");
                return;
            }
            midi_data2 = (DCLANG_LONG) POP;
            midi_data1 = (DCLANG_LONG) POP;
            midi_status = (DCLANG_LONG) POP;
            Pm_WriteShort(
                midi_stream,
                TIME_PROC(TIME_INFO),
                Pm_Message(midi_status, midi_data1, midi_data2)
            );
            NEXT;
        _OP_PM_WSR:
            if (data_stack_ptr < 3)
            {
                printf("_pm_wsr needs 3 integers on the stack:\n");
                printf("    <data2> <data1> <status>\n");
                return;
            }
            midi_status = (DCLANG_LONG) POP;
            midi_data1 = (DCLANG_LONG) POP;
            midi_data2 = (DCLANG_LONG) POP;
            Pm_WriteShort(
                midi_stream,
                0,
                Pm_Message(midi_status, midi_data1, midi_data2)
            );
            NEXT;
        _OP_PM_CLOSE:
            Pm_Close(midi_stream);
            printf("Portmidi port closed.\n");
            NEXT;
        _OP_PM_TERMINATE:
            Pm_Terminate();
            printf("Portmidi process terminated.\n");
            NEXT;
    }
}

DCLANG_LONG dclang_findword(const char *word) {
    for (DCLANG_LONG x = num_user_words - 1; x > -1 ; x--) {
        if (strcmp(user_words[x].name, word) == 0) {
            return user_words[x].word_start;
        }
    }
    return -1;
}

void dclang_callword(DCLANG_PTR where) {
    locals_base_idx += 8;
    // mark where we are for restoration later
    return_stack[return_stack_ptr++] = iptr;
    // set word target; execute word target
    iptr = where;
    dclang_execute();
}

///////////////////////////////////////////
// PRIMITIVES REGISTRATION (AKA opcodes) //
///////////////////////////////////////////

void add_primitive(char *name, char *category, int opcode) {
    primitives_idx += 1;
    (primitives + primitives_idx)->name = name;
    (primitives + primitives_idx)->opcode = opcode;
    (primitives + primitives_idx)->category = category;
};

void add_all_primitives() {
    primitives = (struct primitive *)dclang_malloc(208*sizeof(primitive));
    // stack manipulation
    add_primitive("drop", "Stack Ops", OP_DROP);
    add_primitive("dup", "Stack Ops", OP_DUP);
    add_primitive("over", "Stack Ops", OP_OVER);
    add_primitive("pick", "Stack Ops", OP_PICK);
    add_primitive("swap", "Stack Ops", OP_SWAP);
    add_primitive("2drop", "Stack Ops", OP_DROP2);
    add_primitive("2dup", "Stack Ops", OP_DUP2);
    add_primitive("2over", "Stack Ops", OP_OVER2);
    add_primitive("depth", "Stack Ops", OP_DEPTH);
    add_primitive("clear", "Stack Ops", OP_CLEAR);
    // the extra "save" stack
    add_primitive("svpush", "Save Stack Ops", OP_SVPUSH);
    add_primitive("svpop", "Save Stack Ops", OP_SVPOP);
    add_primitive("svdrop", "Save Stack Ops", OP_SVDROP);
    add_primitive("svpick", "Save Stack Ops", OP_SVPICK);
    add_primitive("svdepth", "Save Stack Ops", OP_SVDEPTH);
    add_primitive("svclear", "Save Stack Ops", OP_SVCLEAR);
    // stack output
    add_primitive(".", "Stack Output", OP_SHOW);
    add_primitive("..", "Stack Output", OP_SHOWNOSPACE);
    add_primitive(".rj", "Stack Output", OP_SHOWRJ);
    add_primitive(".pz", "Stack Output", OP_SHOWPZ);
    add_primitive(".s", "Stack Output", OP_SHOWSTACK);
    // boolean logic
    add_primitive("null", "Boolean", OP_NULL);
    add_primitive("false", "Boolean", OP_FALSE);
    add_primitive("true", "Boolean", OP_TRUE);
    add_primitive("=", "Boolean", OP_EQ);
    add_primitive("<>", "Boolean", OP_NOTEQ);
    add_primitive("<", "Boolean", OP_LT);
    add_primitive(">", "Boolean", OP_GT);
    add_primitive("<=", "Boolean", OP_LTE);
    add_primitive(">=", "Boolean", OP_GTE);
    add_primitive("assert", "Boolean", OP_ASSERT);
    // bit manipulation
    add_primitive("and", "Bit manipulation", OP_AND);
    add_primitive("or", "Bit manipulation", OP_OR);
    add_primitive("xor", "Bit manipulation", OP_XOR);
    add_primitive("not", "Bit manipulation", OP_NOT);
    add_primitive("<<", "Bit manipulation", OP_LSHIFT);
    add_primitive(">>", "Bit manipulation", OP_RSHIFT);
    // basic arithmetic
    add_primitive("+", "Arithmetic", OP_ADD);
    add_primitive("-", "Arithmetic", OP_SUB);
    add_primitive("*", "Arithmetic", OP_MUL);
    add_primitive("/", "Arithmetic", OP_DIV);
    add_primitive("%", "Arithmetic", OP_MOD);
    add_primitive("abs", "Arithmetic", OP_ABS);
    add_primitive("min", "Arithmetic", OP_MIN);
    add_primitive("max", "Arithmetic", OP_MAX);
    // float -> int
    add_primitive("round", "Float -> Integer", OP_ROUND);
    add_primitive("ceil", "Float -> Integer", OP_CEIL);
    add_primitive("floor", "Float -> Integer", OP_FLOOR);
    // higher math
    add_primitive("pow", "Higher Math", OP_POWER);
    add_primitive("sqrt", "Higher Math", OP_SQRT);
    add_primitive("exp", "Higher Math", OP_EXP);
    add_primitive("log", "Higher Math", OP_LOG);
    add_primitive("log2", "Higher Math", OP_LOG2);
    add_primitive("log10", "Higher Math", OP_LOG10);
    add_primitive("e", "Higher Math", OP_E);
    // trig
    add_primitive("pi", "Trigonometry", OP_PI);
    add_primitive("sin", "Trigonometry", OP_SINE);
    add_primitive("cos", "Trigonometry", OP_COS);
    add_primitive("tan", "Trigonometry", OP_TAN);
    add_primitive("asin", "Trigonometry", OP_ASINE);
    add_primitive("acos", "Trigonometry", OP_ACOS);
    add_primitive("atan", "Trigonometry", OP_ATAN);
    add_primitive("atan2", "Trigonometry", OP_ATAN2);
    add_primitive("sinh", "Trigonometry", OP_ATAN);
    add_primitive("cosh", "Trigonometry", OP_ATAN);
    add_primitive("tanh", "Trigonometry", OP_ATAN);
    // randomness
    add_primitive("rand", "Randomness", OP_RAND);
    // variables and arrays
    add_primitive("var", "Variables & Arrays", OP_VARIABLE);
    add_primitive("!", "Variables & Arrays", OP_POKE);
    add_primitive("@", "Variables & Arrays", OP_PEEK);
    add_primitive("allot", "Variables & Arrays", OP_ALLOT);
    add_primitive("create", "Variables & Arrays", OP_CREATE);
    add_primitive(",", "Variables & Arrays", OP_COMMA);
    add_primitive("here", "Variables & Arrays", OP_HERE);
    add_primitive("const", "Variables & Arrays", OP_CONSTANT);
    add_primitive("envget", "Variables & Arrays", OP_ENVGET);
    add_primitive("envset", "Variables & Arrays", OP_ENVSET);
    // sorting
    add_primitive("sortnums", "Array Sorting", OP_SORTNUMS);
    add_primitive("sortstrs", "Array Sorting", OP_SORTSTRS);
    // hash set & get
    add_primitive("h!", "Hashes", OP_HASHSET);
    add_primitive("h@", "Hashes", OP_HASHGET);
    add_primitive("hkeys", "Hashes", OP_HASHKEYS);
    // tree storage
    add_primitive("tmake", "Trees", OP_TREEMAKE);
    add_primitive("t!", "Trees", OP_TREESET);
    add_primitive("t@", "Trees", OP_TREEGET);
    add_primitive("twalk", "Trees", OP_TREEWALK);
    add_primitive("tdel", "Trees", OP_TREEDELETE);
#ifdef HAS_TREEDESTROY
    add_primitive("tdestroy", "Trees", OP_TREEDESTROY);
#endif
    // linked lists
    add_primitive("lmake", "Lists", OP_LISTMAKE);
    add_primitive("_lnext", "Lists", OP_LISTNEXT);
    add_primitive("l!", "Lists", OP_LISTSET);
    add_primitive("l@", "Lists", OP_LISTGET);
    add_primitive("lpush", "Lists", OP_LISTPUSH);
    add_primitive("lpop", "Lists", OP_LISTPOP);
    add_primitive("lins", "Lists", OP_LISTINSERT);
    add_primitive("lrem", "Lists", OP_LISTREMOVE);
    add_primitive("lsize", "Lists", OP_LISTSIZE);
    add_primitive("ldel", "Lists", OP_LISTDELETE);
    // branching/control
    add_primitive("times", "Branching", OP_TIMES);
    add_primitive("again", "Branching", OP_AGAIN);
    add_primitive("exittimes", "Branching", OP_EXITTIMES);
    add_primitive("for", "Branching", OP_FOR);
    add_primitive("next", "Branching", OP_NEXT);
    add_primitive("exitfor", "Branching", OP_EXITFOR);
    add_primitive("i", "Branching", OP_I);
    add_primitive("j", "Branching", OP_J);
    add_primitive("k", "Branching", OP_K);
    add_primitive("if", "Branching", OP_IF);
    add_primitive("else", "Branching", OP_ELSE);
    add_primitive("endif", "Branching", OP_ENDIF);
    add_primitive("return", "Branching", OP_RETURN);
    // time
    add_primitive("clock", "Time", OP_CLOCK);
    add_primitive("epoch", "Time", OP_EPOCH);
    add_primitive("epoch->dt", "Time", OP_EPOCH_TO_DT);
    add_primitive("dt->epoch", "Time", OP_DT_TO_EPOCH);
    add_primitive("sleep", "Time", OP_SLEEP);
    // output and string ops
    add_primitive("cr", "String Output", OP_CR);
    add_primitive("print", "String Output", OP_PRINT);
    // character emitters
    add_primitive("emit", "Character Emitters", OP_EMIT);
    add_primitive("uemit", "Character Emitters", OP_UEMIT);
    add_primitive("bytes32", "Character Emitters", OP_BYTES32);
    // character types
    add_primitive("isalnum", "Character Types", OP_ISALNUM);
    add_primitive("isalpha", "Character Types", OP_ISALPHA);
    add_primitive("iscntrl", "Character Types", OP_ISCNTRL);
    add_primitive("isdigit", "Character Types", OP_ISDIGIT);
    add_primitive("isgraph", "Character Types", OP_ISGRAPH);
    add_primitive("islower", "Character Types", OP_ISLOWER);
    add_primitive("isprint", "Character Types", OP_ISPRINT);
    add_primitive("ispunct", "Character Types", OP_ISPUNCT);
    add_primitive("isspace", "Character Types", OP_ISSPACE);
    add_primitive("isupper", "Character Types", OP_ISUPPER);
    add_primitive("isxdigit", "Character Types", OP_ISXDIGIT);
    // string conversion
    add_primitive("tohex", "String Conversion", OP_TOHEX);
    add_primitive("tostr", "String Conversion", OP_TOSTR);
    add_primitive("tonum", "String Conversion", OP_TONUM);
    add_primitive("ord", "String Conversion", OP_ORD);
    add_primitive("tolower", "String Conversion", OP_TOLOWER);
    add_primitive("toupper", "String Conversion", OP_TOUPPER);
    // general string ops from C
    add_primitive("strlen", "String Ops", OP_STRLEN);
    add_primitive("str=", "String Ops", OP_STREQ);
    add_primitive("str<", "String Ops", OP_STRLT);
    add_primitive("str>", "String Ops", OP_STRGT);
    add_primitive("strfind", "String Ops", OP_STRFIND);
    add_primitive("strspn", "String Ops", OP_STRSPN);
    add_primitive("strcspn", "String Ops", OP_STRCSPN);
    add_primitive("strtok", "String Ops", OP_STRTOK);
    add_primitive("mempcpy", "String Ops", OP_MEMPCPY);
    add_primitive("memset", "String Ops", OP_MEMSET);
    // memory buffers
    add_primitive("mkbuf", "Memory", OP_MKBUF);
    add_primitive("free", "Memory", OP_FREE);
    add_primitive("memused", "Memory", OP_MEMUSED);
    // regex
    add_primitive("regcomp", "Regex", OP_REGCOMP);
    add_primitive("regexec", "Regex", OP_REGEXEC);
    add_primitive("regread", "Regex", OP_REGREAD);
    // file
    add_primitive("import", "Files", OP_IMPORT);
    add_primitive("fopen", "Files", OP_FILEOPEN);
    add_primitive("fmemopen", "Files", OP_FILEMEMOPEN);
    add_primitive("fread", "Files", OP_FILEREAD);
    add_primitive("freadline", "Files", OP_FILEREADLINE);
    add_primitive("freadall", "Files", OP_FILEREADALL);
    add_primitive("fseek", "Files", OP_FILESEEK);
    add_primitive("ftell", "Files", OP_FILETELL);
    add_primitive("fwrite", "Files", OP_FILEWRITE);
    add_primitive("fflush", "Files", OP_FILEFLUSH);
    add_primitive("fclose", "Files", OP_FILECLOSE);
    add_primitive("redirect", "Files", OP_REDIRECT);
    add_primitive("resetout", "Files", OP_RESETOUT);
    add_primitive("flush", "Files", OP_FLUSHOUT);
    // low-level (OS) file ops:
    add_primitive("open", "Files (no buffer)", OP_OPEN);
    add_primitive("read", "Files (no buffer)", OP_READ);
    add_primitive("write", "Files (no buffer)", OP_WRITE);
    add_primitive("close", "Files (no buffer)", OP_CLOSE);
    // tcp/udp networking using sockets
    add_primitive("tcplisten", "Sockets", OP_TCPLISTEN);
    add_primitive("tcpaccept", "Sockets", OP_TCPACCEPT);
    add_primitive("tcpconnect", "Sockets", OP_TCPCONNECT);
    add_primitive("udprecv", "Sockets", OP_UDPRECV);
    add_primitive("udpsend", "Sockets", OP_UDPSEND);
    // block a SIGINT
    add_primitive("block_sigint", "Interrupt Blocking", OP_BLOCKSIGINT);
    add_primitive("unblock_sigint", "Interrupt Blocking", OP_UNBLOCKSIGINT);
    // os fork and exit
    add_primitive("fork", "Operating System", OP_FORK);
    add_primitive("exit", "Operating System", OP_EXIT);
    // info primitives
    add_primitive("primitives", "Info", OP_SHOWPRIMITIVES);
    add_primitive("words", "Info", OP_SHOWWORDS);
    add_primitive("constants", "Info", OP_SHOWCONSTS);
    add_primitive("variables", "Info", OP_SHOWVARS);
    // dynamic input
    add_primitive("exec", "Dynamic Input", OP_EXEC);
    add_primitive("input", "Dynamic Input", OP_INPUT);
    // SQLite3 interface
    add_primitive("_sqlite_open", "SQLite", _OP_SQLITEOPEN);
    add_primitive("_sqlite_prepare", "SQLite", _OP_SQLITEPREPARE);
    add_primitive("_sqlite_step", "SQLite", _OP_SQLITESTEP);
    add_primitive("_sqlite_column", "SQLite", _OP_SQLITECOLUMN);
    add_primitive("_sqlite_finalize", "SQLite", _OP_SQLITEFINALIZE);
    add_primitive("_sqlite_exec", "SQLite", _OP_SQLITEEXEC);
    add_primitive("_sqlite_close", "SQLite", _OP_SQLITECLOSE);
    // portmidi
    add_primitive("_pm_list", "MIDI", _OP_PM_LIST);
    add_primitive("_pm_open_out", "MIDI", _OP_PM_OPENOUT);
    add_primitive("_pm_ws", "MIDI", _OP_PM_WS);
    add_primitive("_pm_wsr", "MIDI", _OP_PM_WSR);
    add_primitive("_pm_close", "MIDI", _OP_PM_CLOSE);
    add_primitive("_pm_terminate", "MIDI", _OP_PM_TERMINATE);
    add_primitive(0, 0, 0);  // end of primitives 'barrier'
};

void _show_primitives() {
    printf("\nThere are currently %i primitives implemented.\n", primitives_idx);
    printf("The following primitives are visible; invisible primitives start "
           "with '_' and are meant to be used privately by libraries:\n"
    );
    const struct primitive *pr = primitives;
    char *old_category = " ";
    char *new_category;
    for (int x=0; x < primitives_idx; x++)
    {
        new_category = (pr + x)->category;
        if (((pr + x)->name)[0] != '_')
        {
            if (strcmp(old_category, new_category))
            {
                printf("\n%-19s| ", new_category);
            }
            printf("%s ", (pr + x)->name);
        }
        old_category = new_category;
    }
    printf("\nWord Definitions   | : ; { } (see documentation)\n");
    printf("\nStrings are written by simply typing a string literal in double-quotes, e.g. \"Hello there!\".\n\n");
    fflush(ofp);
}

////////////////
// REPL STUFF //
////////////////

const char *illegal[] = {"times", "again", "exittimes",
                         "for", "next", "exitfor"};
DCLANG_LONG num_illegal = sizeof(illegal) / sizeof(illegal[0]);

const char *special[] = {"if", "else", "endif"};
DCLANG_LONG num_special = sizeof(special) / sizeof(special[0]);

// function to validate and return an error message if we are using control
// structures outside of a definition
DCLANG_LONG validate(const char *token) {
    DCLANG_LONG checkval = 1;
    for (DCLANG_LONG i=0; i < num_illegal; i++) {
        if (strcmp(token, illegal[i]) == 0) {
            printf("Error: '%s' -- illegal outside of function def.\n",
                   illegal[i]);
            checkval = 0;
            return checkval;
        }
    }
    return checkval;
}

// conditionals are 'special forms' that need to be handled in a certain
//   way by the compilation process:
DCLANG_LONG is_special_form(const char *token) {
    DCLANG_LONG checkval = 0;
    for (DCLANG_LONG i=0; i < num_special; i++) {
        if (strcmp(token, special[i]) == 0) {
            checkval = 1;
            return checkval;
        }
    }
    return checkval;
}

// Function to compile (or interpret) each incoming token
void compile_or_interpret(const char *token) {
    char *endPointer = 0;
    double d;
    const struct primitive *pr = primitives;
    DCLANG_LONG locals_idx = 0;
    DCLANG_LONG const_search_idx = const_idx - 1;
    DCLANG_LONG var_search_idx = var_map_idx - 1;

    if (token == 0) {
        revertinput();
        return;
    }
    // Search user-defined functions (words)
    DCLANG_LONG found = dclang_findword(token);
    if (found != -1) {
        if (def_mode) {
            if (strcmp(user_words[num_user_words - 1].name, token) == 0) {
                prog[iptr].opcode = OP_JUMPU;  // don't overflow the return stack
                prog[iptr++].param = found;
            } else {
                prog[iptr].opcode = OP_CALL;  // normal return stack save
                prog[iptr++].param = found;
            }
        } else {
            dclang_callword(found);
        }
        return;
    }
    // Search for a primitive word
    while (pr->name != 0) {
        if (strcmp(pr->name, token) == 0) {
            if ((def_mode) && (!is_special_form(pr->name))) {
                prog[iptr++].opcode = pr->opcode;
            } else {
                if (validate(token)) {
                    prog[iptr].opcode = pr->opcode;
                    dclang_execute();
                }
            }
            return;
        }
        pr++;
    }
    // Search for a local variable
    if (def_mode) {
        while (locals_idx < 8) {
            if (locals_keys[locals_idx] == NULL) break;
            if (strcmp((char *)locals_keys[locals_idx], token) == 0) {
                prog[iptr].opcode = OP_GET_LOCAL;
                prog[iptr++].param = locals_idx;
                return;
            }
            if ((strncmp((const char *)locals_keys[locals_idx], token, strlen(token)-1) == 0) && (strchr(token, '!') != NULL)) {
                prog[iptr].opcode = OP_SET_LOCAL;
                prog[iptr++].param = locals_idx;
                return;
            }
            locals_idx++;
        }
    }
    // Search for a constant
    while (const_search_idx >= 0) {
        if (strcmp(const_keys[const_search_idx], token) == 0) {
            if (def_mode) {
                prog[iptr].opcode = OP_PUSH;
                prog[iptr++].param = (DCLANG_FLT) const_vals[const_search_idx];
            } else {
                push((DCLANG_FLT) const_vals[const_search_idx]);
            }
            return;
        }
        const_search_idx--;
    }
    // Search for a variable
    while (var_search_idx >= 0) {
        if (strcmp(var_keys[var_search_idx], token) == 0) {
            if (def_mode) {
                prog[iptr].opcode = OP_PUSH;
                prog[iptr++].param = (DCLANG_FLT) var_vals[var_search_idx];
            } else {
                push((DCLANG_FLT) var_vals[var_search_idx]);
            }
            return;
        }
        var_search_idx--;
    }
    // Try to convert to a number
    d = strtod(token, &endPointer);
    if (endPointer != token) {
        if (def_mode) {
            prog[iptr].opcode = OP_PUSH;
            prog[iptr++].param = d;
        } else {
            push(d);
        }
        return;
    }
    // Syntax error handling
    data_stack_ptr = 0;
    printf("%s -- syntax error.\n", token);
    return;
}

void _repl() {
    char *token;
    int locals_idx;
    while (strcmp(token = get_token(), "EOF")) {
        // Are we dealing with a function definition?
        if (strcmp(token, ":") == 0) {
            // grab name
            char *this_token;
            // TODO: validation
            this_token = get_token();
            // put name and current location in user_words lookup array
            user_words[num_user_words].name = this_token;
            user_words[num_user_words++].word_start = iptr;
            def_mode = 1;
            continue; // goto top of loop
        }
        // Are we dealing with locals declaration?
        if (strcmp(token, "{") == 0) {
            if (def_mode) {
                locals_idx = 0;
                while (strcmp(token = get_token(), "}")) {
                    // Add key to ongoing array of keys
                    //printf("Token is: %s\n", token);
                    locals_keys[locals_base_idx + locals_idx] = (DCLANG_PTR)token;
                    locals_idx += 1;
                }
                reverse_array(locals_keys + locals_base_idx, locals_idx - 1);
                // For each key up to the count, pop the stack
                // onto the locals_vals array:
                for (int i=0; i<locals_idx; i++) {
                    prog[iptr].opcode = OP_SET_LOCAL;
                    prog[iptr++].param = (DCLANG_FLT) i;
                }
            } else {
                printf("Illegal use of '{' (locals definition) outside of word definition\n");
            }
            continue; // goto top of loop
        }
        // Are we ending a word?
        if (strcmp(token, ";") == 0) {
            // Simply insert a return call into 'prog' where 'iptr' now points.
            prog[iptr++].opcode = OP_RETURN;
            def_mode = 0;
            continue; // goto top of loop
        }
        // Ok, finaly 'compile' the token, or interpret it on-the-fly
        compile_or_interpret(token);
    }
    compile_or_interpret(0);
}

void _execfunc() {
    if (data_stack_ptr < 1)
    {
        printf("exec -- stack underflow; needs a string representing a word or primitive on the stack! ");
        return;
    }
    DCLANG_PTR string_uint_addr = (DCLANG_PTR) POP;
    if (string_uint_addr < MIN_STR || string_uint_addr > MAX_STR)
    {
        perror("exec -- String address out-of-range.");
        return;
    }
    char *token_arg = (char *)string_uint_addr;
    const struct primitive *pr = primitives;

    // Search user-defined functions (words)
    DCLANG_LONG found = dclang_findword(token_arg);
    if (found != -1) {
        if (def_mode) {
            if (strcmp(user_words[num_user_words - 1].name, token_arg) == 0) {
                prog[iptr].opcode = OP_JUMPU;  // don't overflow the return stack
                prog[iptr++].param = found;
            } else {
                prog[iptr].opcode = OP_CALL;  // normal return stack save
                prog[iptr++].param = found;
            }
        } else {
            dclang_callword(found);
        }
        return;
    }
    // Search for a primitive word
    while (pr->name != 0) {
        if (strcmp(pr->name, token_arg) == 0) {
            if ((def_mode) && (!is_special_form(pr->name))) {
                prog[iptr++].opcode = pr->opcode;
            } else {
                if (validate(token_arg)) {
                    prog[iptr].opcode = pr->opcode;
                    dclang_execute();
                }
            }
            return;
        }
        pr++;
    }
    printf("exec -- word not found: %s\n", token_arg);
    printf("If you call 'exec' repeatedly without using a string constant, "
           "it will result in the string memory getting into a bad state.\n"
           "HINT: try using a constant like: \n"
           "      \"foo\" const :foo \n"
           " and then calling 'exec' with the constant on the stack.\n");
}

void dclang_initialize() {
    setinput(stdin);                       // start input in sane state
    ofp = stdout;                          // start output in sane state
    execfunc = _execfunc;                  // assign the execfunc function to its pointer
    show_primitives = _show_primitives;    // assign the show_primtives function to its pointer
    repl = _repl;                          // assign the repl function to its pointer
    add_all_primitives();                  // register most of the primitives
    srand(time(NULL));                     // seed the random # generator
    global_hash_table = hcreate(1024*256); // create the global hash table
    hashwords = (char**)dclang_malloc(hashwords_size * sizeof(*hashwords));
}
