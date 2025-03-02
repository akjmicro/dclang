/* dclang, an RPN programming language.

'True refinement seeks simplicity.' - Bruce Lee

Born on 2018-05-05 */

#include "dclang.h"

#define push(item) { \
    if (data_stack_ptr >= DATA_STACK_SIZE) { \
        printf("push -- stack overflow!\n"); \
        data_stack_ptr = 0; \
    } \
    data_stack[data_stack_ptr++] = item; \
}

#define push_no_check(item) { \
    data_stack[data_stack_ptr++] = item; \
}

#include "token.c"

DCLANG_FLT dclang_pop()
// special case -- has a return value b/c it can
// be used in other calls to give a value, which
// also means it can be an API call.
{
    return data_stack[--data_stack_ptr];
}


#define NEXT goto  *dispatch_table[prog[++iptr].opcode]

void dclang_execute() {
    DCLANG_FLT val, val1, val2, a, b, c;
    int precision, width;
    DCLANG_PTR env_key_addr, idx, next_var, size;
    DCLANG_ULONG shift_amt, base;
    char *env_key;

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
        &&OP_ENVGET,
        &&OP_ENVSET
    };

    while ((iptr < max_iptr) || (def_mode == 0)) {
        goto *dispatch_table[prog[--iptr].opcode];

    OP_NOOP:
        return;

    // stack ops

    OP_PUSH:
        if (data_stack_ptr >= DATA_STACK_SIZE) {
            printf("push -- stack overflow!\n");
            data_stack_ptr = 0;
        }
        data_stack[data_stack_ptr++] = prog[iptr++].param;
        NEXT;

    OP_PUSH_NO_CHECK:
        data_stack[data_stack_ptr++] = prog[iptr++].param;
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
        DCLANG_PTR pick_idx = (DCLANG_PTR) dclang_pop();
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
        val = dclang_pop();
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
        DCLANG_PTR svpick_idx = (DCLANG_PTR) dclang_pop();
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
        precision = (DCLANG_LONG) dclang_pop();
        width = (DCLANG_LONG) dclang_pop();
        fprintf(ofp, "%*.*f ", width, precision, dclang_pop());
        fflush(ofp);
        NEXT;

    OP_SHOWPZ:
        // left pad with zeros
        if (data_stack_ptr < 3) {
            printf("Stack underflow! '.pz' needs: value, width, precision on the stack\n");
            return;
        }
        precision = (DCLANG_LONG) dclang_pop();
        width = (DCLANG_LONG) dclang_pop();
        fprintf(ofp, "%0*.*f ", width, precision, dclang_pop());
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
    // null (synonymous with 0)
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
            printf("'=' needs two elements on the stack!\n");
            return;
        }
        push(((DCLANG_FLT) dclang_pop() == (DCLANG_FLT) dclang_pop()) * -1);
        NEXT;

    OP_NOTEQ:
        if (data_stack_ptr < 2)
        {
            printf("'!=' needs two elements on the stack!\n");
            return;
        }
        push(((DCLANG_FLT) dclang_pop() != (DCLANG_FLT) dclang_pop()) * -1);
        NEXT;

    OP_GT:
        if (data_stack_ptr < 2)
        {
            printf("'>' needs two elements on the stack!\n");
            return;
        }
        push(((DCLANG_FLT) dclang_pop() < (DCLANG_FLT) dclang_pop()) * -1);
        NEXT;

    OP_LT:
        if (data_stack_ptr < 2)
        {
            printf("'<' needs two elements on the stack!\n");
            return;
        }
        push(((DCLANG_FLT) dclang_pop() > (DCLANG_FLT) dclang_pop()) * -1);
        NEXT;

    OP_GTE:
        if (data_stack_ptr < 2)
        {
            printf("'>=' needs two elements on the stack!\n");
            return;
        }
        push(((DCLANG_FLT) dclang_pop() <= (DCLANG_FLT) dclang_pop()) * -1);
        NEXT;

    OP_LTE:
        if (data_stack_ptr < 2)
        {
            printf("'<=' needs two elements on the stack!\n");
            return;
        }
        push(((DCLANG_FLT) dclang_pop() >= (DCLANG_FLT) dclang_pop()) * -1);
        NEXT;

    // assertions

    OP_ASSERT:
        if (data_stack_ptr < 1)
        {
            printf("'assert' needs an element on the stack!\n");
            return;
        }
        DCLANG_LONG truth = dclang_pop();
        if (truth == 0) {
            printf("ASSERT FAIL!\n");
        }
        NEXT;

    // bitwise operators

    OP_AND:
        if (data_stack_ptr < 2)
        {
            printf("'and' needs two elements on the stack!\n");
            return;
        }
        push((DCLANG_LONG) dclang_pop() & (DCLANG_LONG) dclang_pop());
        NEXT;

    OP_OR:
        if (data_stack_ptr < 2)
        {
            printf("'or' needs two elements on the stack!\n");
            return;
        }
        push((DCLANG_LONG) dclang_pop() | (DCLANG_LONG) dclang_pop());
        NEXT;

    OP_XOR:
        if (data_stack_ptr < 2)
        {
            printf("'xor' needs two elements on the stack!\n");
            return;
        }
        push((DCLANG_LONG) dclang_pop() ^ (DCLANG_LONG) dclang_pop());
        NEXT;

    OP_NOT:
        if (data_stack_ptr < 1)
        {
            printf("'not' needs an element on the stack!\n");
            return;
        }
        push(~(DCLANG_LONG) dclang_pop());
        NEXT;

    OP_LSHIFT:
        if (data_stack_ptr < 2)
        {
            printf("'<<' needs two numbers on the stack!\n");
            return;
        }
        shift_amt = (DCLANG_ULONG) dclang_pop();
        base = (DCLANG_ULONG) dclang_pop();
        push((DCLANG_ULONG) base << shift_amt);
        NEXT;

    OP_RSHIFT:
        if (data_stack_ptr < 2)
        {
            printf("'>>' needs two numbers on the stack!\n");
            return;
        }
        shift_amt = (DCLANG_ULONG) dclang_pop();
        base = (DCLANG_ULONG) dclang_pop();
        push((DCLANG_ULONG) base >> shift_amt);
        NEXT;

    // math
    OP_ADD:
        if (data_stack_ptr < 2)
        {
            printf("'+' needs two numbers on the stack!\n");
            return;
        }
        push(dclang_pop() + dclang_pop());
        NEXT;

    OP_SUB:
        if (data_stack_ptr < 2)
        {
            printf("'-' needs two numbers on the stack!\n");
            return;
        }
        DCLANG_FLT subtrahend = dclang_pop();
        push(dclang_pop() - subtrahend);
        NEXT;

    OP_MUL:
        if (data_stack_ptr < 2)
        {
            printf("'*' needs two numbers on the stack!\n");
            return;
        }
        push(dclang_pop() * dclang_pop());
        NEXT;

    OP_DIV:
        if (data_stack_ptr < 2)
        {
            printf("'/' needs two numbers on the stack!\n");
            return;
        }
        DCLANG_FLT divisor = dclang_pop();
        push(dclang_pop() / divisor);
        NEXT;

    OP_MOD:
        if (data_stack_ptr < 2)
        {
            printf("'%%' needs two numbers on the stack!\n");
            return;
        }
        DCLANG_FLT modulus = dclang_pop();
        push(fmod(dclang_pop(), modulus));
        NEXT;

    OP_ABS:
        if (data_stack_ptr < 1)
        {
            printf("'abs' needs a number on the stack!\n");
            return;
        }
        push(fabs(dclang_pop()));
        NEXT;

    OP_MIN:
        if (data_stack_ptr < 2)
        {
            printf("'min' needs two numbers on the stack!\n");
            return;
        }
        a = dclang_pop();
        b = dclang_pop();
        c = (a < b) ? a : b;
        push(c);
        NEXT;

    OP_MAX:
        if (data_stack_ptr < 2)
        {
            printf("'max' needs two numbers on the stack!\n");
            return;
        }
        a = dclang_pop();
        b = dclang_pop();
        c = (a > b) ? a : b;
        push(c);
        NEXT;

    OP_ROUND:
        if (data_stack_ptr < 1)
        {
            printf("'round' needs a number on the stack!\n");
            return;
        }
        push((DCLANG_LONG) round(dclang_pop()));
        NEXT;

    OP_FLOOR:
        if (data_stack_ptr < 1)
        {
            printf("'floor' needs a number on the stack!\n");
            return;
        }
        push((DCLANG_LONG) floor(dclang_pop()));
        NEXT;

    OP_CEIL:
        if (data_stack_ptr < 1)
        {
            printf("'ceil' needs a number on the stack!\n");
            return;
        }
        push((DCLANG_LONG) ceil(dclang_pop()));
        NEXT;

    // scientific math words

    OP_POWER:
        if (data_stack_ptr < 2)
        {
            printf("'pow' needs two numbers on the stack!\n");
            return;
        }
        DCLANG_FLT raise = dclang_pop();
        push(pow(dclang_pop(), raise));
        NEXT;

    OP_SQRT:
        if (data_stack_ptr < 1)
        {
            printf("'sqrt' needs a number on the stack!\n");
            return;
        }
        push(sqrt(dclang_pop()));
        NEXT;

    OP_EXP:
        if (data_stack_ptr < 1)
        {
            printf("'exp' need a number on the stack!\n");
            return;
        }
        push(exp(dclang_pop()));
        NEXT;

    OP_LOG:
        if (data_stack_ptr < 1)
        {
            printf("'log' needs a number on the stack!\n");
            return;
        }
        push(log(dclang_pop()));
        NEXT;

    OP_LOG2:
        if (data_stack_ptr < 1)
        {
            printf("'log2' needs a number on the stack!\n");
            return;
        }
        push(log2(dclang_pop()));
        NEXT;

    OP_LOG10:
        if (data_stack_ptr < 1)
        {
            printf("'log10' needs a number on the stack!\n");
            return;
        }
        push(log10(dclang_pop()));
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
            printf("'sin' needs a number on the stack!\n");
            return;
        }
        push(sin(dclang_pop()));
        NEXT;

    OP_COS:
        if (data_stack_ptr < 1)
        {
            printf("'cos' needs a number on the stack!\n");
            return;
        }
        push(cos(dclang_pop()));
        NEXT;

    OP_TAN:
        if (data_stack_ptr < 1)
        {
            printf("'tan' needs a number on the stack!\n");
            return;
        }
        push(tan(dclang_pop()));
        NEXT;

    OP_ASINE:
        if (data_stack_ptr < 1)
        {
            printf("'asin' needs a number on the stack!\n");
            return;
        }
        push(asin(dclang_pop()));
        NEXT;

    OP_ACOS:
        if (data_stack_ptr < 1)
        {
            printf("'acos' needs a numbera on the stack!\n");
            return;
        }
        push(acos(dclang_pop()));
        NEXT;

    OP_ATAN:
        if (data_stack_ptr < 1)
        {
            printf("'atan' needs a number on the stack!\n");
            return;
        }
        push(atan(dclang_pop()));
        NEXT;

    OP_ATAN2:
        if (data_stack_ptr < 2)
        {
            printf("'atan2' needs two numbers on the stack!\n");
            return;
        }
        DCLANG_FLT x = dclang_pop();
        DCLANG_FLT y = dclang_pop();
        push(atan2(y, x));
        NEXT;

    OP_SINH:
        if (data_stack_ptr < 1)
        {
            printf("'sinh' needs a number on the stack!\n");
            return;
        }
        push(sinh(dclang_pop()));
        NEXT;

    OP_COSH:
        if (data_stack_ptr < 1)
        {
            printf("'cosh' needs a number on the stack!\n");
            return;
        }
        push(cosh(dclang_pop()));
        NEXT;

    OP_TANH:
        if (data_stack_ptr < 1)
        {
            printf("'tanh' needs a number on the stack!\n");
            return;
        }
        push(tanh(dclang_pop()));
        NEXT;

    // randomness

    OP_RAND:
        push((double)rand()/(double)RAND_MAX);
        NEXT;

    OP_VARIABLE:
        next_var = vars_idx++;
        var_keys[var_map_idx] = get_token();
        var_vals[var_map_idx++] = next_var;
        NEXT;

    OP_POKE:
        if (data_stack_ptr < 2)
        {
            printf("! -- stack underflow! ");
            return;
        }
        idx = (DCLANG_PTR) dclang_pop();
        if (idx < 0 || idx > NUMVARS)
        {
            printf("! -- variable slot number out-of-range!\n");
            return;
        }
        val = dclang_pop();
        vars[idx] = val;
        NEXT;

    OP_PEEK:
        if (data_stack_ptr < 1)
        {
            printf("@ -- stack underflow! ");
            return;
        }
        idx = (DCLANG_PTR) dclang_pop();
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
            printf("allot -- stack underflow! ");
            return;
        }
        vars_idx += (DCLANG_PTR) dclang_pop() - 1;
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
            printf(", -- stack underflow! ");
            return;
        }
        val = dclang_pop();
        vars[vars_idx++] = val;
        NEXT;

    OP_HERE:
        push((DCLANG_PTR) vars_idx);
        NEXT;

    OP_CONSTANT:
        const_keys[const_idx] = get_token();
        const_vals[const_idx++] = dclang_pop();
        NEXT;

    OP_ENVGET:
        if (data_stack_ptr < 1)
        {
            printf("envget -- need <env_key> string on the stack.\n");
        }
        // grab the key
        env_key = (char *)(DCLANG_PTR) dclang_pop();
        env_key_addr = (DCLANG_PTR) env_key;
        if (env_key_addr < MIN_STR || env_key_addr > MAX_STR)
        {
            perror("envget -- String address for hash key out-of-range.");
            return;
        }
        char *val = getenv(env_key);
        DCLANG_PTR val_addr = (DCLANG_PTR) val;
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

    OP_ENVSET:
        if (data_stack_ptr < 2)
        {
            printf("envset -- need <env_val> <env_key> strings on the stack.\n");
        }
        // grab the key from the stack
        env_key = (char *)(DCLANG_PTR) dclang_pop();
        env_key_addr = (DCLANG_PTR) env_key;
        if (env_key_addr < MIN_STR || env_key_addr > MAX_STR)
        {
            perror("envset -- String address for environment key out-of-range.");
            return;
        }
        // grab the value from the stack
        char *env_val = (char *)(DCLANG_PTR) dclang_pop();
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
    }
}

/*


// used by 'freadline' function, which calls 'getline':
char *linebuf = NULL;
size_t linelen = 0;

void fileopenfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("Stack underflow!\n");
        printf("'fopen' needs <filename> <open-mode> on the stack\n");
        return;
    }
    // file mode string
    char *mode = (char *)(DCLANG_PTR) dclang_pop();
    // file path
    char *path = (char *)(DCLANG_PTR) dclang_pop();
    // if mode is read or append, file must exist:
    if ( (access(path, F_OK) == -1)
         && ( !strcmp("r", mode) || !strcmp("r+", mode) ) )
    {
        printf("The file named %s doesn't appear to exist, " \
               "or cannot be accessed.\n", path);
        return;
    }
    FILE *openfptr = fopen(path, mode);
    push((DCLANG_PTR)openfptr);
}

void filememopenfunc()
{
    if (data_stack_ptr < 3)
    {
        printf("Stack underflow!\n");
        printf("'fmemopen' needs <buf (can be 0)> <size> <open-mode> on the stack\n");
        return;
    }
    char *mode = (char *)(DCLANG_PTR) dclang_pop();
    DCLANG_PTR size = (DCLANG_PTR) dclang_pop();
    DCLANG_PTR buf = (DCLANG_PTR) dclang_pop();
    FILE *openfptr = fmemopen(buf, size, mode);
    push((DCLANG_PTR)openfptr);
}

void fileclosefunc()
{
    if (data_stack_ptr < 1)
    {
        printf("Stack underflow!\n");
        printf("'fclose' needs <file_pointer> on the stack\n");
        return;
    }
    FILE *file_to_close = (FILE *)(DCLANG_PTR) dclang_pop();
    fclose(file_to_close);
}

void filereadfunc()
{
    if (data_stack_ptr < 3)
    {
        printf("Stack underflow!\n");
        printf("'fread' needs <buf_pointer> <number-of-bytes> <file_pointer> on the stack\n");
        return;
    }
    FILE *file_to_read = (FILE *)(DCLANG_PTR) dclang_pop();
    DCLANG_LONG num_bytes = (DCLANG_PTR) dclang_pop();
    char *buf = (char *)(DCLANG_PTR) dclang_pop();
    DCLANG_PTR num_bytes_read = fread(buf, 1, num_bytes, file_to_read);
    // update print safety:
    if ((DCLANG_PTR)buf < MIN_STR || MIN_STR == 0)
    {
        MIN_STR = (DCLANG_PTR)buf;
    }
    if ((DCLANG_PTR)buf + num_bytes_read + 1 > MAX_STR || MAX_STR == 0)
    {
        MAX_STR = (DCLANG_PTR)buf + num_bytes_read + 1;
    }
    // push the number of bytes read
    push((DCLANG_LONG) num_bytes_read);
}

void filereadlinefunc()
{
    if (data_stack_ptr < 1)
    {
        printf("Stack underflow!\n");
        printf("'freadline' needs <file_pointer> on the stack\n");
        return;
    }
    FILE *file_to_read = (FILE *)(DCLANG_PTR) dclang_pop();
    DCLANG_PTR num_bytes_read = getline(&linebuf, &linelen, file_to_read);
    // update print safety:
    if ((DCLANG_PTR) linebuf < MIN_STR || MIN_STR == 0)
    {
        MIN_STR = (DCLANG_PTR) linebuf;
    }
    if ((DCLANG_PTR) linebuf + num_bytes_read + 1 > MAX_STR || MAX_STR == 0)
    {
        MAX_STR = (DCLANG_PTR) linebuf + num_bytes_read + 1;
    }
    // push the address of our new string and length
    push((DCLANG_PTR) linebuf);
    // push the number of bytes read
    push((DCLANG_LONG) num_bytes_read);
}

void filereadallfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("Stack underflow!\n");
        printf("'freadall' needs <file_pointer> on the stack\n");
        return;
    }
    DCLANG_PTR chr_cnt = 0;
    DCLANG_PTR bufsize = 64;
    DCLANG_LONG ch;
    char *allbuf = (char *) dclang_malloc(bufsize);
    memset(allbuf, 0, bufsize);
    FILE *file_to_read = (FILE *)(DCLANG_PTR) dclang_pop();
    while ((ch = fgetc(file_to_read)) != EOF)
    {
        chr_cnt += 1;
        if (chr_cnt > bufsize)
        {
            bufsize *= 2;
            allbuf = (char *) dclang_realloc(allbuf, bufsize);
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
}

void fileseekfunc()
{
    if (data_stack_ptr < 3)
    {
        printf("Stack underflow!\n");
        printf("'fseek' needs <offset> <whence> <file_pointer> on the stack\n");
        printf("'Whence' must be 0 (SEEK_SET), 1 (SEEK_CUR), or 2 (SEEK_END).\n");
        return;
    }
    FILE *file_to_seek = (FILE *)(DCLANG_PTR) dclang_pop();
    DCLANG_PTR whence = (DCLANG_PTR) dclang_pop();
    if (!(whence >= 0 && whence <= 2))
    {
        printf("Whence parameter must be between 0 and 2 inclusive!\n");
        return;
    }
    DCLANG_LONG offset = (DCLANG_LONG) dclang_pop();
    fseek(file_to_seek, offset, whence);
}

void filetellfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("Stack underflow!\n");
        printf("'ftell' needs a <file_pointer> on the stack\n");
        return;
    }
    FILE *file_to_tell = (FILE *)(DCLANG_PTR) dclang_pop();
    DCLANG_PTR mylen = ftell(file_to_tell);
    push((DCLANG_PTR) mylen);
}

void filewritefunc()
{
    if (data_stack_ptr < 3)
    {
        printf("'fwrite' -- needs <string-address> <num_of_bytes> <file_pointer> on the stack\n");
        return;
    }
    FILE *file_to_write = (FILE *)(DCLANG_PTR) dclang_pop();
    DCLANG_PTR num_bytes = (DCLANG_PTR) dclang_pop();
    char *str = (char *)(DCLANG_PTR) dclang_pop();
    DCLANG_LONG result = fwrite(str, 1, num_bytes, file_to_write);
    push(result);
}

void fileflushfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'fflush' -- needs <file_pointer> on the stack\n");
        return;
    }
    FILE *file_to_flush = (FILE *)(DCLANG_PTR) dclang_pop();
    fflush(file_to_flush);
}

// lower-level OS calls:

void openfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("Stack_underflow!\n");
        printf("'open' needs <filestr> <flagint> on the stack\n");
        return;
    }
    DCLANG_PTR flagint = (DCLANG_PTR) dclang_pop();
    char *path = (char *)(DCLANG_PTR)dclang_pop();
    int fd = open(path, flagint);
    push((DCLANG_PTR) fd);
}

void readfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("Stack_underflow!\n");
        printf("'read' needs <file_pointer> <buffer-pointer> <numbytes> on the stack\n");
        return;
    }
    DCLANG_PTR numbytes = (DCLANG_PTR) dclang_pop();
    void *buf = (void *)(DCLANG_PTR)dclang_pop();
    int fd = (int) dclang_pop();
    int res = read(fd, buf, numbytes);
    push((int)res);
}

void writefunc()
{
    if (data_stack_ptr < 3)
    {
        printf("Stack_underflow!\n");
        printf("'write' needs <file_pointer> <buffer-pointer> <numbytes> on the stack\n");
        return;
    }
    DCLANG_PTR numbytes = (DCLANG_PTR) dclang_pop();
    void *buf = (void *)(DCLANG_PTR)dclang_pop();
    int fd = (int) dclang_pop();
    int res = write(fd, buf, numbytes);
    push((int)res);
}

void closefunc()
{
    if (data_stack_ptr < 1)
    {
        printf("Stack_underflow!\n");
        printf("'close' needs <file_pointer> on the stack\n");
        return;
    }
    int fp = (int) dclang_pop();
    int res = close(fp);
    push(res);
}
void redirectfunc()
{
    if (data_stack_ptr < 1) {
        printf("Stack underflow! 'redirect' needs an output file pointer before being called\n");
        return;
    }
    ofp = (FILE *)(DCLANG_PTR) dclang_pop();
}
*/

void resetoutfunc()
{
    ofp = stdout;
}

/*
void flushoutfunc()
{
    fflush(ofp);
}


struct sockaddr_in serv_addr, cli_addr;
struct sockaddr_in udp_serv_addr, udp_cli_addr, dest_addr;


void tcplistenfunc()
{
    // Sets up a new listening TCP socket 'object' for listening
    // and returns its address onto the stack
    if (data_stack_ptr < 1) {
        printf("tcplisten -- need <port_number> on the\n");
        return;
    }
    DCLANG_INT sockfd = socket(AF_INET, SOCK_STREAM, 0);
    DCLANG_INT true = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(DCLANG_INT));
    if (sockfd < 0) {
       perror("tcplisten -- ERROR opening socket.");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    DCLANG_INT portno = (DCLANG_INT) dclang_pop();
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
          sizeof(serv_addr)) < 0)
          perror("ERROR on binding");
    listen(sockfd, 5);
    push((DCLANG_INT) sockfd);
}


void tcpacceptfunc()
{
    // Take a given tcplisten-ready socket object and actually make it
    // accept a connection. Returns a handle to the established connection.
    // Can be used inside a loop as a basis for a "multi-connection handling"
    // forking server via `fork`.
    if (data_stack_ptr < 1) {
        printf("tcpaccept -- need <socket> on the stack");
        return;
    }
    DCLANG_UINT clilen = sizeof(cli_addr);
    DCLANG_INT sockfd = (DCLANG_INT) dclang_pop();
    DCLANG_INT newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) perror("tcpaccept -- ERROR on accept step!");
    push((DCLANG_INT) newsockfd);
}


void tcpconnectfunc()
{
    // A tcp client word. This can reach out to a pre-established tcp server
    // already set up with the above words.
    if (data_stack_ptr < 2) {
        printf("tcpconnect -- need <host> <port> on the stack");
        return;
    }
    DCLANG_INT sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) perror("tcpconnect -- ERROR opening socket");
    DCLANG_INT portno = (DCLANG_INT) dclang_pop();
    struct sockaddr_in host_addr;
    char *host = (char *) (DCLANG_PTR) dclang_pop();
    struct hostent *server = gethostbyname(host);
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
}


/////////
// UDP //
/////////

// Possible TODO: make these more 'modular' and configurable w/re: socket opts.

void udprecvfunc() {
    // Receive data over UDP
    if (data_stack_ptr < 3) {
        printf("udprecv -- need <port_number> <max_bytes> <buffer> on the stack\n");
        return;
    }
    // stack values
    char *buffer = (char *) (DCLANG_PTR) dclang_pop();
    DCLANG_INT max_bytes = (DCLANG_INT) dclang_pop();
    DCLANG_INT portno = (DCLANG_INT) dclang_pop();
    // make a socket
    DCLANG_INT sockfd = socket(AF_INET, SOCK_DGRAM, 0);
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
    ssize_t num_bytes = recvfrom(
        sockfd, buffer, max_bytes, 0,
        (struct sockaddr *)&udp_cli_addr, &udp_clilen
    );
    if (num_bytes < 0) {
        perror("udprecv -- ERROR receiving data");
    }
    buffer[num_bytes] = '\0'; // Null terminate the received data
    close(sockfd);
    push((DCLANG_INT) num_bytes);
}


void udpsendfunc() {
    // Send data over UDP to a specified host and port
    if (data_stack_ptr < 3) {
        printf("udpsend -- need <host> <port> <buffer> on the stack\n");
        return;
    }
    // stack values
    char *buffer = (char *) (DCLANG_PTR) dclang_pop();
    DCLANG_INT portno = (DCLANG_INT) dclang_pop();
    char *host = (char *) (DCLANG_PTR) dclang_pop();
    // make a socket
    DCLANG_INT sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("udpsend -- ERROR opening socket");
        return;
    }
    struct hostent *server = gethostbyname(host);
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
    ssize_t num_bytes = sendto(
        sockfd, buffer, strlen(buffer) + 1, 0,
        (struct sockaddr *)&dest_addr, sizeof(dest_addr)
    );
    if (num_bytes < 0) {
        perror("udpsend -- ERROR sending data");
    }
    close(sockfd);
    push((DCLANG_INT) num_bytes);
}
sigset_t block_sigint;

void blocksigintfunc()
{
  sigemptyset(&block_sigint);
  sigaddset(&block_sigint, SIGINT);
  sigprocmask(SIG_BLOCK, &block_sigint, NULL);
}


void unblocksigintfunc()
{
  sigprocmask(SIG_UNBLOCK, &block_sigint, NULL);
}


void forkfunc()
{
    // This function mainly exists so that a multi-client capable tcp/web server
    // can be had. It is assumed that the return value will be caught, inspected,
    // and handled by the caller, so this is really quite a simple c->dclang mapping.
    // TODO: a future enhancement might be to have a counting system in place for
    // avoiding fork-bomb DoS attacks. So, the introspection of a connection limit
    // before granting a new `fork`.
    push((DCLANG_INT) fork());
}


void exitfunc()
{
    if (data_stack_ptr < 1) {
        printf("exit -- need an integer exit code on the stack");
        return;
    }
    DCLANG_INT code = (DCLANG_INT) dclang_pop();
    exit(code);
}


void clockfunc()
{
    gettimeofday(&tval, NULL);
    DCLANG_FLT now = ((DCLANG_FLT) tval.tv_sec) + (((DCLANG_FLT) tval.tv_usec) / 1000000);
    push(now);
}

void epochfunc()
{
    gettimeofday(&tval, NULL);
    DCLANG_FLT now = (tval.tv_sec);
    push(now);
}

void sleepfunc() {
    if (data_stack_ptr < 1) {
        printf("sleep -- need a time amount in seconds on the stack!\n");
        return;
    }
    DCLANG_FLT sleeptime = dclang_pop();
    struct timespec t1, t2;
    t1.tv_sec = floor(sleeptime);
    t1.tv_nsec = round(fmod(sleeptime, 1) * 1000000000);
    nanosleep(&t1, &t2);
}

// date functions

void dt_to_epochfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("dt->epoch: need a <date> like \"2020-01-01 12:14:13\" and a <input_format> on the stack.\n");
        return;
    }
    // input string setup
    DCLANG_ULONG fmt = (DCLANG_ULONG) dclang_pop();
    DCLANG_ULONG to_conv = (DCLANG_ULONG) dclang_pop();
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
}

void epoch_to_dtfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("epoch->dt: need a <epoch_int> and an <output_format> on the stack.\n");
        return;
    }
    // input string setup
    DCLANG_ULONG fmt = (DCLANG_ULONG) dclang_pop();
    if (fmt < MIN_STR || fmt > MAX_STR)
    {
        printf("epoch->dt -- <output_format> string address out-of-range.\n");
        return;
    }
    DCLANG_ULONG in_epoch_uint = (DCLANG_ULONG) dclang_pop();
    time_t in_epoch = (time_t) in_epoch_uint;
    char tmbuf[256];
    memset(&tmbuf[0], 0, 256);
    struct tm *loctime = localtime(&in_epoch);
    if (strftime(tmbuf, 256, (char *) fmt, loctime) == 0)
    {
        printf("'strftime', a low-level call of 'epoch->dt', returned an error.\n");
        return;
    }
    DCLANG_PTR bufaddr = (DCLANG_PTR) tmbuf;
    if (bufaddr > MAX_STR || MAX_STR == 0)
    {
        MAX_STR = bufaddr;
    }
    if (bufaddr < MIN_STR || MIN_STR == 0)
    {
        MIN_STR = bufaddr;
    }
    push(bufaddr);
}
*/


//////////////////////////////////////////////////////////////////////////
// Character emitters. No value goes to the stack; output is immediate. //
//////////////////////////////////////////////////////////////////////////

/*
void emitfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("emit -- stack underflow! ");
        return;
    }
    char char_code = (char) dclang_pop();
    fprintf(ofp, "%c", char_code);
    fflush(ofp);
}

void uemitfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("uemit -- stack underflow! ");
        return;
    }
    long unsigned long char_code = (long unsigned long) dclang_pop();
    long ulen = utf8_encode(utf8_buf, char_code);
    fprintf(ofp, "%s", utf8_buf);
    fflush(ofp);
}

void bytes32func()
{
    DCLANG_INT val = (DCLANG_INT) dclang_pop();
    char low = (char) val & 0xff;
    val >>= 8;
    char lowmid = (char) val & 0xff;
    val >>= 8;
    char highmid = (char) val & 0xff;
    val >>= 8;
    char high = (char) val & 0xff;
    fputc(low, ofp);
    fputc(lowmid, ofp);
    fputc(highmid, ofp);
    fputc(high, ofp);
}

///////////////////////
// character testers //
///////////////////////

void isalnumfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("isalnum -- stack underflow! ");
        return;
    }
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("isalnum -- String address out-of-range.");
        return;
    }
    char *char_ptr = (char *) string_PTR_addr;
    if (isalnum(char_ptr[0])) {
        push((DCLANG_LONG) -1);
    } else {
        push((DCLANG_LONG) 0);
    }
}

void isalphafunc()
{
    if (data_stack_ptr < 1)
    {
        printf("isalpha -- stack underflow! ");
        return;
    }
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("isalpha -- String address out-of-range.");
        return;
    }
    char *char_ptr = (char *) string_PTR_addr;
    if (isalpha(char_ptr[0])) {
        push((DCLANG_LONG) -1);
    } else {
        push((DCLANG_LONG) 0);
    }
}

void iscntrlfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("iscntrl -- stack underflow! ");
        return;
    }
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("iscntrl -- String address out-of-range.");
        return;
    }
    char *char_ptr = (char *) string_PTR_addr;
    if (iscntrl(char_ptr[0])) {
        push((DCLANG_LONG) -1);
    } else {
        push((DCLANG_LONG) 0);
    }
}

void isdigitfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("isdigit -- stack underflow! ");
        return;
    }
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("isdigit -- String address out-of-range.");
        return;
    }
    char *char_ptr = (char *) string_PTR_addr;
    if (isdigit(char_ptr[0])) {
        push((DCLANG_LONG) -1);
    } else {
        push((DCLANG_LONG) 0);
    }
}

void isgraphfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("isgraph -- stack underflow! ");
        return;
    }
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("isgraph -- String address out-of-range.");
        return;
    }
    char *char_ptr = (char *) string_PTR_addr;
    if (isgraph(char_ptr[0])) {
        push((DCLANG_LONG) -1);
    } else {
        push((DCLANG_LONG) 0);
    }
}

void islowerfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("islower -- stack underflow! ");
        return;
    }
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("islower -- String address out-of-range.");
        return;
    }
    char *char_ptr = (char *) string_PTR_addr;
    if (islower(char_ptr[0])) {
        push((DCLANG_LONG) -1);
    } else {
        push((DCLANG_LONG) 0);
    }
}

void isprintfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("isprint -- stack underflow! ");
        return;
    }
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("isprint -- String address out-of-range.");
        return;
    }
    char *char_ptr = (char *) string_PTR_addr;
    if (isprint(char_ptr[0])) {
        push((DCLANG_LONG) -1);
    } else {
        push((DCLANG_LONG) 0);
    }
}

void ispunctfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("ispunct -- stack underflow! ");
        return;
    }
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("ispunct -- String address out-of-range.");
        return;
    }
    char *char_ptr = (char *) string_PTR_addr;
    if (ispunct(char_ptr[0])) {
        push((DCLANG_LONG) -1);
    } else {
        push((DCLANG_LONG) 0);
    }
}

void isspacefunc()
{
    if (data_stack_ptr < 1)
    {
        printf("isspace -- stack underflow! ");
        return;
    }
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("isspace -- String address out-of-range.");
        return;
    }
    char *char_ptr = (char *) string_PTR_addr;
    if (isspace(char_ptr[0])) {
        push((DCLANG_LONG) -1);
    } else {
        push((DCLANG_LONG) 0);
    }
}

void isupperfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("isupper -- stack underflow! ");
        return;
    }
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("isupper -- String address out-of-range.");
        return;
    }
    char *char_ptr = (char *) string_PTR_addr;
    if (isupper(char_ptr[0])) {
        push((DCLANG_LONG) -1);
    } else {
        push((DCLANG_LONG) 0);
    }
}

void isxdigitfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("isxdigit -- stack underflow! ");
        return;
    }
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("isxdigit -- String address out-of-range.");
        return;
    }
    char *char_ptr = (char *) string_PTR_addr;
    if (isxdigit(char_ptr[0])) {
        push((DCLANG_LONG) -1);
    } else {
        push((DCLANG_LONG) 0);
    }
}


void printfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("print -- stack underflow!\n");
        return;
    }
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr == 0)
    {
        printf("print -- Nothing to print.\n");
        return;
    }
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("print -- String address out-of-range.\n");
        return;
    }
    fprintf(ofp, "%s", (char *) string_PTR_addr);
    fflush(ofp);
}

void crfunc()
{
    fprintf(ofp, "\n");
}

//num to hex string, e.g. 0x73af
void tohexfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("tohex -- stack underflow! Needs a number on the stack\n");
        return;
    }
    DCLANG_LONG val = (DCLANG_LONG) dclang_pop();
    int bufsize = snprintf(NULL, 0, "0x%.2lx", val);
    char *str = dclang_malloc(bufsize + 1);
    snprintf(str, bufsize + 1, "0x%.2lx", val);
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) str;
    if (string_PTR_addr < MIN_STR || MIN_STR == 0)
    {
        MIN_STR = string_PTR_addr;
    }
    if (string_PTR_addr + bufsize + 1 > MAX_STR || MAX_STR == 0)
    {
        MAX_STR = string_PTR_addr + bufsize + 1;
    }
    push((DCLANG_PTR) string_PTR_addr);
}

//num to string
void tostrfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("tostr -- needs a number on stack!\n");
        return;
    }
    DCLANG_FLT var = dclang_pop();
    int bufsize = snprintf(NULL, 0, "%g", var);
    char *str = dclang_malloc(bufsize + 1);
    snprintf(str, bufsize + 1, "%g", var);
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) str;
    if (string_PTR_addr < MIN_STR || MIN_STR == 0)
    {
        MIN_STR = string_PTR_addr;
    }
    if (string_PTR_addr + bufsize + 1 > MAX_STR || MAX_STR == 0)
    {
        MAX_STR = string_PTR_addr + bufsize + 1;
    }
    push((DCLANG_PTR) string_PTR_addr);
}

// string to number
void tonumfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("tonum -- needs a <str-pointer> on stack!\n");
        return;
    }
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("tonum -- String address out-of-range.\n");
        return;
    }
    char *mystr = (char *) string_PTR_addr;
    DCLANG_FLT num = strtod(mystr, NULL);
    push(num);
}

// character to number
void ordfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("ord -- stack underflow! Needs a single character in double-quotes on the stack.\n");
        return;
    }
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < (DCLANG_PTR) &memory_pool)
    {
        perror("ord -- String address out-of-range.\n");
        return;
    }
    char *string_loc = (char *) string_PTR_addr;
    push((int) *string_loc);
}

void tolowerfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("tolower -- needs a <source_str> pointer on the stack!\n");
        return;
    }
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("tolower -- String address out-of-range.\n");
        return;
    }
    char *mystr = (char *) string_PTR_addr;
    DCLANG_PTR buflen = (DCLANG_PTR) strlen(mystr);
    char *buf = (char *) dclang_malloc(buflen);
    DCLANG_PTR string_dest_PTR = (DCLANG_PTR) buf;
    int i = 0;
    int c = 0;
    while(*(mystr + i)) {
      c = (int) *(mystr + i);
      memset(buf + i, tolower(c), 1);
      i++;
    }
    if (string_dest_PTR < MIN_STR || MIN_STR == 0)
    {
        MIN_STR = string_dest_PTR;
    }
    if (string_dest_PTR + buflen > MAX_STR || MAX_STR == 0)
    {
        MAX_STR = string_dest_PTR + buflen;
    }
    push((DCLANG_PTR) buf);
}

void toupperfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("toupper -- needs a <source_str> pointer on the stack!\n");
        return;
    }
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("toupper -- String address out-of-range.\n");
        return;
    }
    char *mystr = (char *) string_PTR_addr;
    DCLANG_PTR buflen = (DCLANG_PTR) strlen(mystr);
    char *buf = (char *) dclang_malloc(buflen);
    DCLANG_PTR string_dest_PTR = (DCLANG_PTR) buf;
    int i = 0;
    int c = 0;
    while(*(mystr + i)) {
      c = (int) *(mystr + i);
      memset(buf + i, toupper(c), 1);
      i++;
    }
    if (string_dest_PTR < MIN_STR || MIN_STR == 0)
    {
        MIN_STR = string_dest_PTR;
    }
    if (string_dest_PTR + buflen > MAX_STR || MAX_STR == 0)
    {
        MAX_STR = string_dest_PTR + buflen;
    }
    push((DCLANG_PTR) buf);
}

/////////////////////////////////////
// Standard libc string operations //
/////////////////////////////////////

void strlenfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("strlen -- stack underflow!\n");
        return;
    }
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("strlen -- String address out-of-range.\n");
        return;
    }
    char *mystr = (char *) string_PTR_addr;
    push((DCLANG_PTR) strlen(mystr));
}

void streqfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("str= -- stack underflow!\n");
        return;
    }
    DCLANG_PTR string_PTR_addr2 = (DCLANG_PTR) dclang_pop();
    DCLANG_PTR string_PTR_addr1 = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr1 < MIN_STR || string_PTR_addr1 > MAX_STR)
    {
        perror("strlen -- First given string address out-of-range.\n");
        return;
    }
    if (string_PTR_addr2 < MIN_STR || string_PTR_addr2 > MAX_STR)
    {
        perror("strlen -- Second given string address out-of-range.\n");
        return;
    }
    char *str1 = (char *) string_PTR_addr1;
    char *str2 = (char *) string_PTR_addr2;
    push(((DCLANG_LONG) strcmp(str1, str2) == 0) * -1);
}

void strltfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("str< -- stack underflow!\n");
        return;
    }
    DCLANG_PTR string_PTR_addr2 = (DCLANG_PTR) dclang_pop();
    DCLANG_PTR string_PTR_addr1 = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr1 < MIN_STR || string_PTR_addr1 > MAX_STR)
    {
        perror("str< -- First given string address out-of-range.\n");
        return;
    }
    if (string_PTR_addr2 < MIN_STR || string_PTR_addr2 > MAX_STR)
    {
        perror("str< -- Second given string address out-of-range\n");
        return;
    }
    char *str1 = (char *) string_PTR_addr1;
    char *str2 = (char *) string_PTR_addr2;
    push(((DCLANG_LONG) strcmp(str1, str2) < 0) * -1);
}

void strgtfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("str> -- stack underflow!\n");
        return;
    }
    DCLANG_PTR string_PTR_addr2 = (DCLANG_PTR) dclang_pop();
    DCLANG_PTR string_PTR_addr1 = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr1 < MIN_STR || string_PTR_addr1 > MAX_STR)
    {
        perror("str> -- First given string address out-of-range\n");
        return;
    }
    if (string_PTR_addr2 < MIN_STR || string_PTR_addr2 > MAX_STR)
    {
        perror("str> -- Second given string address out-of-range.\n");
        return;
    }
    char *str1 = (char *) string_PTR_addr1;
    char *str2 = (char *) string_PTR_addr2;
    push(((DCLANG_LONG) strcmp(str1, str2) > 0) * -1);
}

void strfindfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("strfind -- needs <haystack> <needle> string pointers on stack!\n");
        return;
    }
    DCLANG_PTR string_PTR_addr2 = (DCLANG_PTR) dclang_pop();
    DCLANG_PTR string_PTR_addr1 = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr1 < MIN_STR || string_PTR_addr1 > MAX_STR)
    {
        perror("strfind -- 'haystack' (first) string address out-of-range.\n");
        return;
    }
    if (string_PTR_addr2 < MIN_STR || string_PTR_addr2 > MAX_STR)
    {
        perror("strfind -- 'needle' (second) string address out-of-range.\n");
        return;
    }
    char *str1 = (char *) string_PTR_addr1;
    char *str2 = (char *) string_PTR_addr2;
    push((DCLANG_LONG) strstr(str1, str2));
}

void strspnfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("strspn -- needs <str> <test_chars_str> pointers on the stack!\n");
        return;
    }
    DCLANG_PTR delim = (DCLANG_PTR) dclang_pop();
    DCLANG_PTR str = (DCLANG_PTR) dclang_pop();
    if ((delim != 0) && (delim < MIN_STR || delim > MAX_STR))
    {
        perror("strspn -- <delim> string address out-of-range.\n");
        return;
    }
    if (str < MIN_STR || str > MAX_STR)
    {
        perror("strspn -- <str> string address out-of-range.\n");
        return;
    }
    push((DCLANG_LONG) strspn((char *)str, (char *)delim));
}

void strcspnfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("strcspn -- needs <str> <test_chars_str> pointers on the stack!\n");
        return;
    }
    DCLANG_PTR delim = (DCLANG_PTR) dclang_pop();
    DCLANG_PTR str = (DCLANG_PTR) dclang_pop();
    if ((delim != 0) && (delim < MIN_STR || delim > MAX_STR))
    {
        perror("strcspn -- <test_chars_str> string address out-of-range.\n");
        return;
    }
    if (str < MIN_STR || str > MAX_STR)
    {
        perror("strcspn -- <str> string address out-of-range.");
        return;
    }
    push((DCLANG_LONG) strcspn((char *)str, (char *)delim));
}

void strtokfunc()
{
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
    DCLANG_PTR savepoint = (DCLANG_PTR) dclang_pop();
    char **savepoint_ptr = (char **) &vars[savepoint];
    DCLANG_PTR delim = (DCLANG_PTR) dclang_pop();
    DCLANG_PTR str = (DCLANG_PTR) dclang_pop();
    if ((str != 0) && (str < MIN_STR || str > MAX_STR))
    {
        perror("strtok -- <str> (first) string address out-of-range.\n");
        return;
    }
    if (delim < MIN_STR || delim > MAX_STR)
    {
        perror("strtok -- <delim> (second) string address out-of-range.\n");
        return;
    }
    push((DCLANG_LONG) strtok_r((char *)str, (char *)delim, savepoint_ptr));
}

void mempcpyfunc()
{
    if (data_stack_ptr < 3)
    {
        printf("mempcpy -- needs <dest> <source> <size> on stack!\n");
        return;
    }
    DCLANG_PTR size = (DCLANG_PTR) dclang_pop();
    DCLANG_PTR source = (DCLANG_PTR) dclang_pop();
    DCLANG_PTR dest = (DCLANG_PTR) dclang_pop();
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
}

void memsetfunc()
{
    if (data_stack_ptr < 3)
    {
        printf("memset -- needs <dest_str> <char-int> <times-int> on stack!\n");
        return;
    }
    DCLANG_PTR times = (DCLANG_PTR) dclang_pop();
    DCLANG_PTR chr = (DCLANG_PTR) dclang_pop();
    DCLANG_PTR dest = (DCLANG_PTR) dclang_pop();
    if ((dest != 0) && (dest < MIN_STR || dest > MAX_STR))
    {
        perror("memset -- <dest> string address out-of-range.\n");
        return;
    }
    push((DCLANG_PTR)memset((char *)dest, (int)chr, (int)times));
}

////////////////////
// memory buffers //
////////////////////

void mkbuffunc()
{
    if (data_stack_ptr < 1)
    {
        printf("Stack_underflow!\n");
        printf("'mkbuf' needs <size-as-integer> on the stack\n");
    }
    DCLANG_PTR size = (DCLANG_PTR) dclang_pop();
    char *buf = (char *) dclang_malloc(size);
    memset(buf, 0, size);
    DCLANG_PTR advance = (DCLANG_PTR) strlen(buf);
    DCLANG_PTR bufaddr = (DCLANG_PTR) buf;
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
}

void freefunc()
{
    if (data_stack_ptr < 1)
    {
        printf("free -- stack underflow! N.B. This word is actually a no-op, kept for backwards compatibility\n");
        return;
    }
    DCLANG_PTR loc_PTR = (DCLANG_PTR) dclang_pop();
    dclang_free((char *) loc_PTR);
}

void memusedfunc()
{
    DCLANG_FLT memused = (DCLANG_FLT) (((float) unused_mem_idx) / ((float) MEMSIZE));
    push(memused);
}
// Function to compile a regex pattern
void regcompfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("regcomp -- stack underflow: need <pattern> and <flags> on the stack!\n");
        return;
    }

    // Pop the regex pattern from the stack
    DCLANG_ULONG flags = (DCLANG_ULONG)dclang_pop();
    DCLANG_PTR pattern_PTR_addr = (DCLANG_PTR)dclang_pop();

    if (pattern_PTR_addr < MIN_STR || pattern_PTR_addr > MAX_STR)
    {
        perror("regcomp -- Pattern address out-of-range.\n");
        return;
    }

    char *pattern = (char *)pattern_PTR_addr;

    // Compile the regex pattern
    regex_t *regex = (regex_t *)dclang_malloc(sizeof(regex_t));
    if (regcomp(regex, pattern, (int)flags) != 0)
    {
        perror("regcomp -- Error compiling regex pattern\n");
        dclang_free(regex);
        return;
    }

    // Push the compiled regex object pointer onto the stack
    push((DCLANG_PTR)regex);
}

// Function to execute the regex matching
void regexecfunc()
{
    if (data_stack_ptr < 3)
    {
        printf("regexec -- stack underflow; need <regexobj> <string_to_search> <flags> on the stack!\n");
        return;
    }

    // Pop the input string and the compiled regex object from the stack
    DCLANG_LONG flags = (DCLANG_LONG)dclang_pop();
    DCLANG_PTR input_str_PTR_addr = (DCLANG_PTR)dclang_pop();
    DCLANG_PTR regex_obj_PTR = (DCLANG_PTR)dclang_pop();

    if (regex_obj_PTR < 0 || input_str_PTR_addr < MIN_STR || input_str_PTR_addr > MAX_STR)
    {
        perror("regexec -- Invalid regex object or string address.\n");
        return;
    }

    regex_t *regex = (regex_t *)regex_obj_PTR;
    char *input_str = (char *)input_str_PTR_addr;

    // Execute the regex matching
    regmatch_t *match = (regmatch_t *)dclang_malloc(10 * sizeof(regmatch_t));
    if (regexec(regex, input_str, 10, match, (int)flags) == 0)
    {
        // If a match is found, push the match object onto the stack
        push((DCLANG_PTR)match);
    }
    else
    {
        // No match found, push -1 to indicate failure
        push((DCLANG_LONG)-1);
    }
}

void regreadfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("regread -- stack underflow; need <regexec_result> <match_index> on the stack! \n");
        printf("regread will return a start and end index relative to the original searched string. \n");
        printf("If the user actually wants that substring, it makes sense to have saved the original \n");
        printf("string, and put the results of regread at the top of stack, then call `strslice`,\n");
        printf("which needs to be imported from the 'string' module.\n");
        return;
    }

    DCLANG_LONG index = (DCLANG_LONG)dclang_pop();
    DCLANG_PTR regmatch_pnt = (DCLANG_PTR)dclang_pop();

    if ((DCLANG_LONG)regmatch_pnt > 0) {
        regmatch_t *match = (regmatch_t *)regmatch_pnt;
        push((DCLANG_LONG)(match[index].rm_so));
        push((DCLANG_LONG)(match[index].rm_eo));
    } else {
        push((DCLANG_LONG)-1);
        push((DCLANG_LONG)-1);
    }
}
*/


/*
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

// looping
void timesfunc()
{
    return_stack[return_stack_ptr++] = iptr;
    times_info[times_ptr++] = (DCLANG_LONG) dclang_pop();
    loop_counter[loop_counter_ptr++] = 0;
}

void _conttimes()
{
    loop_counter[loop_counter_ptr - 1] += 1;
    iptr = return_stack[return_stack_ptr - 1];
}

void exittimesfunc()
{
    loop_counter[--loop_counter_ptr] = 0;
    --return_stack_ptr;
    --times_ptr;
}

void againfunc()
{
    if (loop_counter[loop_counter_ptr - 1] < times_info[times_ptr - 1] - 1) {
        _conttimes();
    } else {
        exittimesfunc();
    }
}

// these 'for' loops are more flexible, allowing from/to/step parameters.
void forfunc()
{
    return_stack[return_stack_ptr++] = iptr;
    fl_stack[fl_ptr].step = (DCLANG_LONG) dclang_pop();
    loop_counter[loop_counter_ptr++] = (DCLANG_LONG) dclang_pop();
    fl_stack[fl_ptr++].limit = (DCLANG_LONG) dclang_pop();
}

void _contfor()
{
    loop_counter[loop_counter_ptr - 1] += fl_stack[fl_ptr - 1].step;
    iptr = return_stack[return_stack_ptr - 1];
}

void exitforfunc()
{
    --fl_ptr;
    loop_counter[--loop_counter_ptr] = 0;
    --return_stack_ptr;
}

void nextfunc()
{
    if (fl_stack[fl_ptr - 1].step > 0) {
        if (loop_counter[loop_counter_ptr - 1] < \
                (fl_stack[fl_ptr - 1].limit \
                 - fl_stack[fl_ptr - 1].step)) {
            _contfor();
        } else {
            exitforfunc();
        }
    } else {
        if (loop_counter[loop_counter_ptr - 1] > \
                (fl_stack[fl_ptr - 1].limit \
                 - fl_stack[fl_ptr - 1].step)) {
            _contfor();
        } else {
            exitforfunc();
        }
    }
}

void ifunc()
{
    push_no_check(loop_counter[loop_counter_ptr - 1]);
}

void jfunc()
{
    push_no_check(loop_counter[loop_counter_ptr - 2]);
}

void kfunc()
{
    push_no_check(loop_counter[loop_counter_ptr - 3]);
}


// jump if zero (false)
void jumpzfunc(DCLANG_FLT where)
{
    DCLANG_LONG truth = (DCLANG_LONG) dclang_pop();
    if (!truth) {
        iptr = (uintptr_t) where;
    }
}

// unconditional jump
void jumpufunc(DCLANG_FLT where)
{
    iptr = (uintptr_t) where;
}

// if-else-endif
void iffunc()
{
    // mark our location
    return_stack[return_stack_ptr++] = iptr;
    // set jump location for 'else'...w/o 'where' location
    // will be filled in by 'else'
    prog[iptr].function.with_param = jumpzfunc;
    prog[iptr++].param = 0;
}

void elsefunc()
{
    // get the last starting point of the 'if' clause
    DCLANG_LONG if_val = return_stack[--return_stack_ptr];
    // mark out current location on the return stack
    return_stack[return_stack_ptr++] = iptr;
    // set the unconditional jump, but no 'where' yet
    // (will be filled in later by 'endif')....
    prog[iptr].function.with_param = jumpufunc;
    prog[iptr++].param = 0;
    // update old if val goto:
    prog[if_val].param = iptr;
}

void endiffunc()
{
    DCLANG_LONG last_val = return_stack[--return_stack_ptr];
    prog[last_val].param = iptr;
}
*/

/* Here we add the functionality necessary for the user to define named
 procedures (words). The idea is to have a struct that contains:
 1) a string naming the word.
 2) the position of that word in the 'prog' array.

 The difference between this and the already defined primitives is that the
 2nd element in the primitives struct was a function pointer.  Here, it is
 an index (which is also a pointer) to the start of a procedure, which
 itself may contain other procedures that the code may jump to, or simply
 resolve to some primitives.

 In the 'prog' array, which is indexed by 'iptr', in effect, a word call
 is simply a jump to the start of that procedure.  When the 'compile'
 function finds that a user-defined bit of code is being referenced, what is
 put into 'prog' is a call to 'callfunc', with the parameter to that call
 being the index in 'prog' where the word resides. */

/*
// for debugging
void showdefined()
{
    for (int x=0; x < num_user_words; x++) {
        printf("Word %i: %s @ %li\n", x, user_words[x].name,\
                                         user_words[x].word_start);
    }
}

DCLANG_LONG dclang_findword(const char *word)
{
    for (DCLANG_LONG x = num_user_words - 1; x > -1 ; x--) {
        if (strcmp(user_words[x].name, word) == 0) {
            return user_words[x].word_start;
        }
    }
    return -1;
}

void callword(DCLANG_FLT where)
{
    locals_base_idx += 8;
    // mark where we are for restoration later
    return_stack[return_stack_ptr++] = iptr;
    // set word target; execute word target
    iptr = (DCLANG_PTR) where;
    (*(prog[iptr].function.with_param)) (prog[iptr++].param);
    // adjust the locals base pointer
}

void dclang_callword(DCLANG_PTR where)
{
    callword((DCLANG_FLT) where);
    // execute all until we reach the end of the iptr queue
    while (iptr < max_iptr) {
        (*(prog[iptr].function.with_param)) (prog[iptr++].param);
    }
}

void returnfunc()
{
    // restore locals_base_idx
    locals_base_idx -= 8;
    // restore the old iptr
    iptr = return_stack[--return_stack_ptr];
    // adjust the locals pointers
}

// respond to ':' token:
void startword()
{
    // grab name
    char *this_token;
    // TODO: validation
    this_token = get_token();
    // put name and current location in user_words lookup array
    user_words[num_user_words].name = this_token;
    user_words[num_user_words++].word_start = iptr;
}

// respond to ';' token:
void endword()
{
    // Simply insert a return call into 'prog' where 'iptr' now points.
    prog[iptr++].function.without_param = returnfunc;
    max_iptr = iptr;
}

#include "noheap/ht.c"

ht *global_hash_table;

////////////
// Locals //
////////////

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

void _set_locals_var(DCLANG_FLT flt_idx)
{
    int idx = (int) flt_idx;
    locals_vals[locals_base_idx + idx] = dclang_pop();
}

void _processlocals()
{
    char *token;
    int locals_idx;
    while (strcmp(token = get_token(), "}")) {
        // Add key to ongoing array of keys
        //printf("Token is: %s\n", token);
        locals_keys[locals_base_idx + locals_idx] = token;
        locals_idx += 1;
    }
    reverse_array(locals_keys + locals_base_idx, locals_idx - 1);
    // For each key up to the count, pop the stack
    // onto the locals_vals array:
    for (int i=0; i<locals_idx; i++) {
        prog[iptr].function.with_param = _set_locals_var;
        prog[iptr++].param = (DCLANG_FLT) i;
    }
}

void _get_locals_var(DCLANG_FLT flt_idx)
{
    int idx = (int) flt_idx;
    push(locals_vals[locals_base_idx + idx]);
}

/////////////
// Globals //
/////////////


// some helpers to show stuff

void showvars()
{
    for (int x=0; x < var_map_idx; x++) {
        printf("Variable %i: %s @ %li\n", x, var_keys[x], var_vals[x]);
    }
}

void showconsts()
{
    for (int x=0; x < const_idx; x++) {
        printf("Constant %i: %s ==> %.19g\n", x, const_keys[x], const_vals[x]);
    }
}

//////////////////////////////////////////////////////////
// global hash space, a la 'redis', but in local memory //
//////////////////////////////////////////////////////////

void _add_key(char *key){
    if (hashwords_cnt > hashwords_size)
    {
        hashwords_size *= 2;
        hashwords = dclang_realloc(hashwords, hashwords_size * sizeof(*hashwords));
    }
    hashwords[hashwords_cnt] = key;
    ++hashwords_cnt;
}

void hashsetfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("h! -- stack underflow! ");
        return;
    }
    // grab the key
    char *key = (const char *)(DCLANG_PTR) dclang_pop();
    DCLANG_PTR key_addr = (DCLANG_PTR) key;
    if (key_addr < MIN_STR || key_addr > MAX_STR)
    {
        perror("h! -- String address for hash key out-of-range.");
        return;
    }
    // grab the value
    void *value = (void *)(DCLANG_PTR) dclang_pop();
    void *confirm = hset(global_hash_table, key, value);
    if (confirm != NULL)
    {
        _add_key(key);
    }
}

void hashgetfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("h@ -- stack underflow! ");
        return;
    }
    // grab the key
    char *key = (char *)(DCLANG_PTR) dclang_pop();
    DCLANG_PTR key_addr = (DCLANG_PTR) key;
    if (key_addr < MIN_STR || key_addr > MAX_STR)
    {
        perror("h@ -- String address for hash key out-of-range.");
        return;
    }
    // grab the value
    void *value = hget(global_hash_table, key);
    if (value == NULL)
    {
        push(0);
    } else {
        push((DCLANG_PTR)(char *)value);
    }
}

void hashkeysfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("hkeys -- need an integer index on stack. Stack underflow! ");
        return;
    }
    // grab the key index
    DCLANG_PTR keyidx = (DCLANG_PTR) dclang_pop();
    push((DCLANG_PTR) hashwords[keyidx]);
}

// helper functions for sorting

int compare_doubles (const void *a, const void *b)
{
    const double *da = (const double *) a;
    const double *db = (const double *) b;
    return (*da > *db) - (*da < *db);
}

int compare_strings (const void *a, const void *b)
{
    const char *sa = (const char *) (DCLANG_PTR) * (DCLANG_FLT *) a;
    const char *sb = (const char *) (DCLANG_PTR) * (DCLANG_FLT *) b;
    return strcmp(sa, sb);
}

// end helper functions

void sortnumsfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("sortnums -- need <arrstart_index> <size> on the stack.\n");
        return;
    }
    int size = (DCLANG_PTR) dclang_pop();
    int arrstart = (DCLANG_PTR) dclang_pop();
    qsort (vars+arrstart, size, sizeof(DCLANG_FLT), compare_doubles);
}

void sortstrsfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("sortstrs -- need <arrstart_index> <size> on the stack.\n");
    }
    int size = (DCLANG_PTR) dclang_pop();
    int arrstart = (DCLANG_PTR) dclang_pop();
    qsort (vars+arrstart, size, sizeof(DCLANG_FLT), compare_strings);
}

// environment variables:

#include "noheap/trees.c"

/* main stuct used for a dictionary-style tree entry.
   'key' is a string, 'value' is a double, which can represent
   either a number or a string pointer.

   It is up to the user to supply checks and use the types
   safely. */

/*
struct tree_entry
{
    char *key;
    DCLANG_FLT value;
};

struct tree_entry *
make_tree_entry(char *key, DCLANG_FLT value)
{
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

void *tree_roots[NUM_TREE_ROOTS] = {NULL};
int tree_roots_idx = -1;

void treemakefunc()
{
    tree_roots_idx += 1;
    if (tree_roots_idx > (NUM_TREE_ROOTS - 1))
    {
        printf("tmake -- tree root limit reached!\n");
        printf("You can change the limit when compiling via the NUM_TREE_ROOTS variable\n");
        return;
    }
    push((DCLANG_PTR) tree_roots_idx);
}

int tree_compare_func(const void *l, const void *r)
{
    if (l == NULL || r == NULL) return 0;
    struct tree_entry *tree_l = (struct tree_entry *)l;
    struct tree_entry *tree_r = (struct tree_entry *)r;
    return strcmp(tree_l->key, tree_r->key);
}

void treegetfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("t@ -- stack underflow! Need <tree_root> <keystr> on the stack.\n");
        printf("You can make a root node via 'tmake', and assign it to a variable ");
        printf("so it can be referred to later.\n");
        return;
    }

    // Pop args
    char *search_key = (char *)(DCLANG_PTR) dclang_pop();
    DCLANG_PTR tree_idx = (DCLANG_PTR) dclang_pop();
    struct tree_entry dummy_tree;
    dummy_tree.key = search_key;
    dummy_tree.value = 0;
    struct tree_entry *retval = tfind(&dummy_tree, &tree_roots[tree_idx], tree_compare_func);
    if (retval == NULL)
    {
        push((DCLANG_PTR) 0);
        return;
    }
    push((DCLANG_FLT)((*(struct tree_entry **)retval)->value));
}

void treesetfunc()
{
    if (data_stack_ptr < 3)
    {
        printf("t! -- stack underflow! Need <tree_root> <keystr> <val> on the stack.\n");
        printf("You can make a root node via 'tmake', and assign it to a variable ");
        printf("so it can be referred to later.\n");
        return;
    }

    // Pop args
    DCLANG_FLT value = dclang_pop();
    char *search_key = (char *)(DCLANG_PTR) dclang_pop();
    DCLANG_PTR tree_idx = (DCLANG_PTR) dclang_pop();

    struct tree_entry *te_del = make_tree_entry(dclang_strdup(search_key), value);
    tdelete(te_del, &tree_roots[tree_idx], tree_compare_func);
    struct tree_entry *te = make_tree_entry(dclang_strdup(search_key), value);
    struct tree_entry *retval = tsearch(te, &tree_roots[tree_idx], tree_compare_func);
    push((DCLANG_FLT)((*(struct tree_entry **)retval)->value));
}

// helper used by `treewalk`
void print_node(const void *node, const VISIT order, const int depth)
{
    if (order == preorder || order == leaf ) {
		    printf(
		        "key=%s, value=%s\n",
		        (*(struct tree_entry **)node)->key,
		        (char *)(DCLANG_PTR)((*(struct tree_entry **)node)->value)
		    );
    }
}

void treewalkfunc()
{
    if (data_stack_ptr < 1) {
        printf("twalk -- stack underflow! Need <tree_root> on the stack.\n");
        return;
    }
    DCLANG_PTR tree_idx = (DCLANG_PTR) dclang_pop();
    twalk(tree_roots[tree_idx], print_node);
}

void treedeletefunc()
{
    if (data_stack_ptr < 2) {
        printf("tdel -- stack underflow! Need <tree_root> <key> on the stack.\n");
        return;
    }

    // Pop args
    char *key = (char *)(DCLANG_PTR) dclang_pop();
    DCLANG_PTR tree_idx = (DCLANG_PTR) dclang_pop();

    struct tree_entry *te_del = make_tree_entry(dclang_strdup(key), 0);
    tdelete(te_del, &tree_roots[tree_idx], tree_compare_func);
    dclang_free(te_del);
}

#ifdef HAS_TREEDESTROY
void treedestroyfunc()
{
    if (data_stack_ptr < 1) {
        printf("tdestroy -- stack underflow! Need <tree_root> on the stack.\n");
        return;
    }
    DCLANG_PTR tree_idx = (DCLANG_PTR) dclang_pop();
    tdestroy(tree_roots[tree_idx], dclang_free);
    tree_roots[tree_idx] = NULL;
}
#endif

#include "noheap/llist.c"

// Define the structure for linked list nodes
struct Node {
    struct Node *next;
    struct Node *prev;
    DCLANG_FLT data;  // New member for data
};

// Function to create a linked list with a single node containing user-chosen data
void listmakefunc() {
    // Allocate memory for the head node
    struct Node *list = (struct Node *)dclang_malloc(sizeof(struct Node));

    // Set the head node to point to itself in both directions
    list->next = list;
    list->prev = list;

    // Push the pointer to the head node onto the stack
    push((DCLANG_PTR)list);
}

void listnextfunc() {
    if (data_stack_ptr < 1) {
        printf("_lnext -- stack underflow; need <existing_list_node> on the stack! ");
        return;
    }
    DCLANG_PTR list_ptr = (DCLANG_PTR)dclang_pop();
    // Convert pointers to the actual node structure
    struct Node *list = (struct Node *)list_ptr;
    struct Node *next = list->next;
    push((DCLANG_PTR) next);
}

// Function to append a node to the tail of the linked list
void listpushfunc() {
    if (data_stack_ptr < 2) {
        printf("lpush -- stack underflow; need <list> <value> on the stack! ");
        return;
    }

    // Pop args
    DCLANG_FLT value = dclang_pop();
    DCLANG_PTR list_ptr = (DCLANG_PTR)dclang_pop();

    // Convert pointers to the actual node structure
    struct Node *list = (struct Node *)list_ptr;

    // Allocate memory for the new node
    struct Node *new_node = (struct Node *)dclang_malloc(sizeof(struct Node));

    // Initialize the new node
    new_node->data = value;

    // Insert the new node before the head node (at the tail)
    insque(new_node, list->prev);
}

// Function to pop a node from the tail of the linked list
void listpopfunc() {
    if (data_stack_ptr < 1) {
        printf("lpop -- stack underflow; need <list> on the stack! ");
        return;
    }

    // Pop arg
    DCLANG_PTR list_ptr = (DCLANG_PTR)dclang_pop();

    // Convert pointers to the actual node structure
    struct Node *list = (struct Node *)list_ptr;

    // Check if the list is empty
    if (list->next == list) {
        printf("lpop -- list is empty! ");
        return;
    }

    // Get the tail node
    struct Node *tail_node = list->prev;

    // Remove the tail node from the list
    remque(tail_node);

    // Push the data of the tail node onto the stack
    push((DCLANG_FLT)tail_node->data);

    // Free the memory of the popped node
    dclang_free(tail_node);
}

// Function to set the data of a node in the linked list
void listsetfunc() {
    if (data_stack_ptr < 3) {
        printf("l! -- stack underflow; need <list> <slot> <value> on the stack! ");
        return;
    }

    // Pop args
    DCLANG_FLT value = dclang_pop();
    DCLANG_LONG slot = (DCLANG_LONG)dclang_pop();
    DCLANG_PTR list_ptr = (DCLANG_PTR)dclang_pop();

    // Convert pointers to the actual node structure
    struct Node *list = (struct Node *)list_ptr;

    // Find the Nth node in the linked list
    struct Node *node = list;
    for (int i = 0; i < slot + 1; i++) {
        if (node->next == list) {
            // Reached the end of the list
            printf("l! -- slot out of bounds! ");
            return;
        }
        node = node->next;
    }

    // Set the data of the node
    node->data = value;
}

// Function to get the data of a node in the linked list
void listgetfunc() {
    if (data_stack_ptr < 2) {
        printf("l@ -- stack underflow; need <list> <slot> on the stack! ");
        return;
    }

    // Pop args
    DCLANG_LONG slot = (DCLANG_LONG)dclang_pop();
    DCLANG_PTR list_ptr = (DCLANG_PTR)dclang_pop();

    // Convert pointers to the actual node structure
    struct Node *list = (struct Node *)list_ptr;

    // Find the Nth node in the linked list
    struct Node *node = list;
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
}


// Function to insert a node into a linked list before a specified node
void listinsertfunc() {
    if (data_stack_ptr < 3) {
        printf("lins -- stack underflow; need <list> <node_slot> <value> on the stack! ");
        return;
    }

    // Pop args
    DCLANG_FLT value = dclang_pop();
    DCLANG_LONG node_slot = (DCLANG_LONG)dclang_pop();
    DCLANG_PTR list_ptr = (DCLANG_PTR)dclang_pop();

    // Convert pointers to the actual node structure
    struct Node *list = (struct Node *)list_ptr;

    // Allocate memory for the new node
    struct Node *new_node = (struct Node *)dclang_malloc(sizeof(struct Node));

    // Initialize the new node
    new_node->data = value;

    // Find the node before which to insert the new node
    struct Node *node_before = list;
    for (int i = 0; i < node_slot; i++) {
        if (node_before->next == list) {
            // Reached the end of the list
            printf("lins -- node_slot out of bounds! ");
            dclang_free(new_node);
            return;
        }
        node_before = node_before->next;
    }

    // Insert the new node before the specified node
    insque(new_node, node_before);
}

// Function to remove a node from a linked list at a specified node slot
void listremovefunc() {
    if (data_stack_ptr < 2) {
        printf("lrem -- stack underflow; need <list> <node_slot> on the stack! ");
        return;
    }

    // Pop the node slot and list pointer and node slot from the stack
    DCLANG_LONG node_slot = (DCLANG_LONG)dclang_pop();
    DCLANG_PTR list_ptr = (DCLANG_PTR)dclang_pop();

    // Convert pointers to the actual node structure
    struct Node *list = (struct Node *)list_ptr;

    // Find the node to remove
    struct Node *node_to_remove = list;
    for (int i = 0; i < node_slot + 1; i++) {
        if (node_to_remove->next == list) {
            // Reached the end of the list
            printf("lrem -- node_slot out of bounds! ");
            return;
        }
        node_to_remove = node_to_remove->next;
    }

    // Remove the specified node from the list
    remque(node_to_remove);

    // Free the memory of the removed node
    dclang_free(node_to_remove);
}

// Function to get the size (number of nodes) in a linked list
void listsizefunc() {
    if (data_stack_ptr < 1) {
        printf("lsize -- stack underflow; need <list> on the stack! ");
        return;
    }

    // Pop the list pointer from the stack
    DCLANG_PTR list_ptr = dclang_pop();

    // Convert pointer to the actual node structure
    struct Node *list = (struct Node *)list_ptr;

    // Initialize the size counter
    int size = 0;

    // Traverse the linked list and count each node
    struct Node *current = list->next;
    while (current != list) {
        size++;
        current = current->next;
    }

    // Push the size onto the stack
    push((DCLANG_LONG)size);
}


// Function to delete (dclang_free) all nodes in a linked list
void listdeletefunc() {
    if (data_stack_ptr < 1) {
        printf("ldel -- stack underflow; need <list> on the stack! ");
        return;
    }

    // Pop the list pointer from the stack
    DCLANG_PTR list_ptr = (DCLANG_PTR)dclang_pop();

    // Convert pointer to the actual node structure
    struct Node *list = (struct Node *)list_ptr;

    // Traverse the linked list and free each node in the 'next' direction
    struct Node *current_next = list->next;
    while (current_next != list) {
        struct Node *next_node = current_next->next;
        dclang_free(current_next);
        current_next = next_node;
    }

    // Free the head node
    // Reset the list to a blank head node
    list->next = list;
    list->prev = list;
}


// will be the private pointer to the working MIDI stream
PmStream *midi_stream;
#define TIME_PROC ((int32_t (*)(void *)) Pt_Time)
#define TIME_INFO NULL

void _pm_listfunc()
{
    // list device information
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
}

void _pm_openoutfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'_pm_open_out' needs a device number on the stack!\n");
        return;
    }
    DCLANG_LONG device = (DCLANG_LONG) dclang_pop();
    Pm_OpenOutput(&midi_stream, device, NULL, 0, NULL, NULL , 0);
}

void _pm_wsfunc()
{
    if (data_stack_ptr < 3)
    {
        printf("'_pm_ws' needs 3 integers on the stack:\n");
        printf("    <status> <data1> <data2>\n");
        return;
    }
    DCLANG_LONG data2 = (DCLANG_LONG) dclang_pop();
    DCLANG_LONG data1 = (DCLANG_LONG) dclang_pop();
    DCLANG_LONG status = (DCLANG_LONG) dclang_pop();
    Pm_WriteShort(
        midi_stream,
        TIME_PROC(TIME_INFO),
        Pm_Message(status, data1, data2)
    );
}

void _pm_wsrfunc()
{
    if (data_stack_ptr < 3)
    {
        printf("'_pm_wsr' needs 3 integers on the stack:\n");
        printf("    <data2> <data1> <status>\n");
        return;
    }
    DCLANG_LONG status = (DCLANG_LONG) dclang_pop();
    DCLANG_LONG data1 = (DCLANG_LONG) dclang_pop();
    DCLANG_LONG data2 = (DCLANG_LONG) dclang_pop();
    Pm_WriteShort(
        midi_stream,
        0,
        Pm_Message(status, data1, data2)
    );
}

void _pm_closefunc()
{
    Pm_Close(midi_stream);
    printf("Portmidi port closed.\n");
}

void _pm_terminatefunc()
{
    Pm_Terminate();
    printf("Portmidi process terminated.\n");
}


// Wrapper function for sqlite3_open
void _sqliteopenfunc()
{
    const char* db_path = (const char*)(DCLANG_PTR)dclang_pop();
    sqlite3* db;
    int rc = sqlite3_open(db_path, &db);
    if (rc != SQLITE_OK) {
        printf("SQLite3 error: %s\n", sqlite3_errmsg(db));
        return;
    }
    push((DCLANG_PTR)db);
}

// Wrapper function for sqlite3_prepare
void _sqlitepreparefunc() {
    char *sql = (char *)(DCLANG_PTR)dclang_pop();
    sqlite3* db = (sqlite3 *)(DCLANG_PTR)dclang_pop();
    const char *sql_const = (const char *) sql;
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql_const, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("SQLite3 error: %s\n", sqlite3_errmsg(db));
        return;
    }
    push((DCLANG_PTR)stmt);
}

// Wrapper function for sqlite3_step
void _sqlitestepfunc() {
    sqlite3_stmt* stmt = (sqlite3_stmt *)(DCLANG_PTR)dclang_pop();
    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
        printf("SQLite3 error: %s\n", sqlite3_errmsg(sqlite3_db_handle(stmt)));
        return;
    }
    push((DCLANG_PTR)rc);
}

// Wrapper function for sqlite3_column
void _sqlitecolumnfunc() {
    int col_index = (int)dclang_pop();
    sqlite3_stmt* stmt = (sqlite3_stmt *)(DCLANG_PTR)dclang_pop();
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
}

// Wrapper function for sqlite3_finalize
void _sqlitefinalizefunc() {
    sqlite3_stmt* stmt = (sqlite3_stmt *)(DCLANG_PTR)dclang_pop();
    int rc = sqlite3_finalize(stmt);
    if (rc != SQLITE_OK) {
        printf("SQLite3 error: %s\n", sqlite3_errmsg(sqlite3_db_handle(stmt)));
        return;
    }
}

// _sqliteexecfunc is a convenience wrapper
static int __sqlcallback(void *NotUsed, int argc, char **argv, char **azColName)
{
  int i;
  for(i=0; i<argc; i++){
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

void _sqliteexecfunc()
{
    char *zErrMsg = 0;
    char *sql = (char *)(DCLANG_PTR)dclang_pop();
    sqlite3* db = (sqlite3 *)(DCLANG_PTR)dclang_pop();
    int rc = sqlite3_exec(db, sql, __sqlcallback, 0, &zErrMsg);
    if(rc != SQLITE_OK) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
}

// Wrapper function for sqlite3_close
void _sqliteclosefunc() {
    sqlite3* db = (sqlite3 *)(DCLANG_PTR)dclang_pop();
    int rc = sqlite3_close(db);
    if (rc != SQLITE_OK) {
        printf("SQLite3 error: %s\n", sqlite3_errmsg(db));
        return;
    }
}
*/

//////////////////////////////////////////////
// Registration of primitives (AKA opcodes) //
//////////////////////////////////////////////

void add_primitive(char *name, char *category, int opcode)
{
    primitives_idx += 1;
    (primitives + primitives_idx)->name = name;
    (primitives + primitives_idx)->opcode = opcode;
    (primitives + primitives_idx)->category = category;
};

void add_all_primitives()
{
    primitives = dclang_malloc(208*sizeof(primitive));
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
    /*
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
    // output and string ops
    add_primitive("cr", "String Output", OP_CR);
    add_primitive("print", "String Output", OP_PRINT);
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
    // SQLite3 interface
    add_primitive("_sqlite_open", "SQLite", _OP_SQLITEOPEN);
    add_primitive("_sqlite_prepare", "SQLite", _OP_SQLITEPREPARE);
    add_primitive("_sqlite_step", "SQLite", _OP_SQLITESTEP);
    add_primitive("_sqlite_column", "SQLite", _OP_SQLITECOLUMN);
    add_primitive("_sqlite_finalize", "SQLite", _OP_SQLITEFINALIZE);
    add_primitive("_sqlite_exec", "SQLite", _OP_SQLITEEXEC);
    add_primitive("_sqlite_close", "SQLite", _OP_SQLITECLOSE);
    // tcp/udp networking using sockets
    add_primitive("tcplisten", "Sockets", OP_TCPLISTEN);
    add_primitive("tcpaccept", "Sockets", OP_TCPACCEPT);
    add_primitive("tcpconnect", "Sockets", OP_TCPCONNECT);
    add_primitive("udprecv", "Sockets", OP_UDPRECV);
    add_primitive("udpsend", "Sockets", OP_UDPSEND);
    // time
    add_primitive("clock", "Time", OP_CLOCK);
    add_primitive("sleep", "Time", OP_SLEEP);
    add_primitive("epoch", "Time", OP_EPOCH);
    add_primitive("dt->epoch", "Time", dt_to_OP_EPOCH);
    add_primitive("epoch->dt", "Time", epoch_to_OP_DT);
    // block a SIGINT
    add_primitive("block_sigint", "Interrupt Blocking", OP_BLOCKSIGINT);
    add_primitive("unblock_sigint", "Interrupt Blocking", OP_UNBLOCKSIGINT);
    // portmidi
    add_primitive("_pm_list", "MIDI", _OP_PM_LIST);
    add_primitive("_pm_open_out", "MIDI", _OP_PM_OPENOUT);
    add_primitive("_pm_ws", "MIDI", _OP_PM_WS);
    add_primitive("_pm_wsr", "MIDI", _OP_PM_WSR);
    add_primitive("_pm_close", "MIDI", _OP_PM_CLOSE);
    add_primitive("_pm_terminate", "MIDI", _OP_PM_TERMINATE);
    // os fork and exit
    add_primitive("fork", "Operating System", OP_FORK);
    add_primitive("exit", "Operating System", OP_EXIT);
    // show defined words!
    add_primitive("words", "Other", OP_SHOWDEFINED);
    add_primitive("constants", "Other", OP_SHOWCONSTS);
    add_primitive("variables", "Other", OP_SHOWVARS);
    */
    add_primitive(0, 0, 0);
};

void show_primitivesfunc()
{
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
    printf(": ; { }\n\n");
    printf("Strings are written by simply typing a string literal in double-quotes, e.g. \"Hello there!\".\n\n");
}


const char *illegal[] = {"times", "again", "exittimes",
                         "for", "next", "exitfor"};
DCLANG_LONG num_illegal = sizeof(illegal) / sizeof(illegal[0]);

const char *special[] = {"if", "else", "endif"};
DCLANG_LONG num_special = sizeof(special) / sizeof(special[0]);


/* function to validate and return an error message if we are using control
 * structures outside of a definition */
DCLANG_LONG validate(const char *token)
{
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


/* conditionals are 'special forms' that need to be handled in a certain
   way by the compilation process: */
DCLANG_LONG is_special_form(const char *token)
{
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
void compile_or_interpret(const char *token)
{
    char *endPointer = 0;
    double d;
    const struct primitive *pr = primitives;
    DCLANG_LONG locals_idx = 0;
    DCLANG_LONG const_search_idx = const_idx - 1;
    DCLANG_LONG var_search_idx = var_map_idx - 1;

    if (token == 0) {
        return;
    }

    /*
    // Search user-defined functions (words)
    DCLANG_LONG found = dclang_findword(token);
    if (found != -1) {
        if (def_mode) {
            if (strcmp(user_words[num_user_words - 1].name, token) == 0) {
                prog[iptr].opcode = OP_JUMP;  // don't overflow the return stack
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
    */

    // Search for a primitive word
    while (pr->name != 0) {
        if (strcmp(pr->name, token) == 0) {
            if (def_mode) {
                if (!is_special_form(pr->name)) {
                    prog[iptr++].opcode = pr->opcode;
                }
            } else {
                if (validate(token)) {
                    prog[iptr++].opcode = pr->opcode;
                    dclang_execute();
                }
            }
            return;
        }
        pr++;
    }

    /*
    // Search for a local variable
    if (def_mode) {
        while (locals_idx < 8) {
            if (strcmp(locals_keys[locals_idx], token) == 0) {
                prog[iptr].opcode = OP_GET_LOCAL;
                prog[iptr++].param = locals_idx;
                return;
            }
            if ((strncmp(locals_keys[locals_idx], token, strlen(token)-1) == 0) && (strchr(token, '!') != NULL)) {
                prog[iptr].opcode = OP_SET_LOCAL;
                prog[iptr++].param = locals_idx;
                return;
            }
            locals_idx++;
        }
    }
    */

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
            prog[iptr].opcode = OP_PUSH;
            prog[iptr++].param = d;
            dclang_execute();
        }
        return;
    }

    // Syntax error handling
    data_stack_ptr = 0;
    printf("%s -- syntax error.\n", token);
    return;
}


void repl() {
    char *token;
    while (strcmp(token = get_token(), "EOF")) {
        /*
        // are we dealing with a function definition?
        if (strcmp(token, ":") == 0) {
            startword();
            def_mode = 1;
            continue; // goto top of loop
        }
        if (strcmp(token, "{") == 0) {
            if (def_mode) {
                _processlocals();
            } else {
                printf("Illegal use of '{' (locals definition) outside of word definition\n");
            }
            continue; // goto top of loop
        }
        if (strcmp(token, ";") == 0) {
            endword();
            def_mode = 0;
            continue; // goto top of loop
        }
        */
        // 'compile' it, or interpret it on-the-fly
        compile_or_interpret(token);
    }
    compile_or_interpret(0);
}

////////////////////////////
// Dealing with importing //
////////////////////////////
/*
DCLANG_LONG dclang_import(char *infilestr) {
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


int importfunc() {
    if (data_stack_ptr < 1) {
        printf("import -- stack underflow! ");
        return -1;
    }
    char *importfile = (char *)(unsigned long) dclang_pop();
    return dclang_import(importfile);
}
*/

/*
// a small buffer for use by `grabinput`
char string_pad[512];
int string_here;

void grabinput() {
    setinput(stdin);
    char ch;
    // get a starting marker for length
    unsigned long string_start = string_here;
    // bypass leading whitespace
    do {
        if((ch = fgetc(ifp)) == EOF) {
            exit(0);
        }
    } while(isspace(ch));
    while (! strchr("\n", ch)) {
        if (strchr("\\", ch)) {
            // consume an extra char due to backslash
            if ((ch = getchar()) == EOF) exit(0);
            // backspace
            if (strchr("b", ch)) {
                ch = 8;
            }
            if (strchr("n", ch)) {
                ch = 10;
            }
            if (strchr("t", ch)) {
                ch = 9;
            }
        }
        string_pad[string_here++] = ch;
        if ((ch = getchar()) == EOF) exit(0);
    }
    DCLANG_PTR string_addr = (DCLANG_PTR) string_start;
    DCLANG_PTR string_size = (DCLANG_PTR)(string_here - string_start);
    char *string_dest = dclang_malloc(string_size + 1);
    char nullstr[] = "\0";
    memcpy(string_dest, (char *)((DCLANG_PTR)&string_pad[0] + string_addr), string_size);
    DCLANG_PTR string_dest_PTR = (DCLANG_PTR) string_dest;
    if (string_dest_PTR < MIN_STR || MIN_STR == 0) {
        MIN_STR = string_dest_PTR;
    }
    if (string_dest_PTR + string_size + 1 > MAX_STR || MAX_STR == 0) {
        MAX_STR = string_dest_PTR + string_size + 1;
    }
    push(string_dest_PTR);
    revertinput();
}

void inputfunc() {
    if (def_mode) {
        prog[iptr++].function.without_param = grabinput;
    } else {
        grabinput();
    }
}
*/

/*
void execfunc() {
    if (data_stack_ptr < 1)
    {
        printf("exec -- stack underflow! ");
        return;
    }
    DCLANG_PTR string_uint_addr = (DCLANG_PTR) dclang_pop();
    if (string_uint_addr < MIN_STR || string_uint_addr > MAX_STR)
    {
        perror("exec -- String address out-of-range.");
        return;
    }
    char *argument = (char *)string_uint_addr;
    const struct primitive *pr = primitives;

    // search user-defined functions (words)
    DCLANG_LONG found = dclang_findword(argument);
    if (found > -1) {
        if (def_mode) {
            prog[iptr].function.with_param = callword;
            prog[iptr++].param = found;
        } else {
            dclang_callword(found);
        }
        return;
    }

    // search dictionary (list, not hash) entry
    while (pr->name != 0) {
        if (strcmp(pr->name, argument) == 0) {
            if ((def_mode) && (!is_special_form(pr->name))) {
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
    printf("exec -- word not found: %s\n", argument);
    printf("If you call 'exec' repeatedly without using a string constant, "
           "it will result in the string memory getting into a bad state.\n"
           "HINT: try using a constant like: \n"
           "      \"foo\" const :foo \n"
           " and then calling 'exec' with the constant on the stack.\n");
}
*/

/*
// needed so we can add 'import' to primitives
void load_extra_primitives()
{
    add_primitive("primitives", "Other", show_primitivesfunc);
    add_primitive("import", "Other", importfunc);
    add_primitive("input", "Other", inputfunc);
    add_primitive("exec", "Other", execfunc);
    // final endpoint must be zeros,
    // and they won't count in the 'count':
    add_primitive(0, 0, 0);
}
*/

void dclang_initialize()
{
    setinput(stdin);
    resetoutfunc();
    add_all_primitives();
    //load_extra_primitives();
    srand(time(NULL));
    // create the global hash table
    //global_hash_table = hcreate(1024*256);
    //hashwords = (char**)dclang_malloc(hashwords_size * sizeof(*hashwords));
}
