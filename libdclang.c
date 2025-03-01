/* dclang, an RPN programming language.

'True refinement seeks simplicity.' - Bruce Lee

Born on 2018-05-05 */

#include "dclang.h"

/* stack operations */
void push(DCLANG_FLT a)
{
    if (data_stack_ptr >= DATA_STACK_SIZE) {
        printf("push -- stack overflow!\n");
        data_stack_ptr = 0;
    }
    data_stack[data_stack_ptr++] = a;
}

void push_no_check(DCLANG_FLT a)
{
    data_stack[data_stack_ptr++] = a;
}

DCLANG_FLT dclang_pop()
// special case -- has a return value b/c it can
// be used in other calls to give a value, which
// also means it can be an API call.
{
    return data_stack[--data_stack_ptr];
}

void dropfunc()
{
    if (data_stack_ptr < 1) {
        printf("drop -- stack underflow!\n");
        return;
    }
    --data_stack_ptr;
}

void dupfunc()
{
    if (data_stack_ptr < 1) {
        printf("dup -- stack underflow!\n");
        return;
    }
    push(data_stack[data_stack_ptr - 1]);
}

void overfunc()
{
    if (data_stack_ptr < 2) {
        printf("over -- stack underflow!\n");
        return;
    }
    push(data_stack[data_stack_ptr - 2]);
}

void pickfunc()
{
    if (data_stack_ptr < 1) {
         printf("pick -- stack underflow!\n");
         return;
    }
    DCLANG_PTR pick_idx = (DCLANG_PTR) dclang_pop();
    if (data_stack_ptr < (pick_idx + 1)) {
        printf("pick -- stack not deep enough!\n");
        return;
    }
    push(data_stack[data_stack_ptr - (pick_idx + 1)]);
}

void swapfunc()
{
    if (data_stack_ptr < 2) {
        printf("swap -- stack underflow!\n");
        return;
    }
    DCLANG_FLT val1 = dclang_pop();
    DCLANG_FLT val2 = dclang_pop();
    push_no_check(val1);
    push_no_check(val2);
}

void drop2func()
{
    if (data_stack_ptr < 2) {
        printf("2drop -- stack underflow!\n");
        return;
    }
    --data_stack_ptr;
    --data_stack_ptr;
}

void dup2func()
{
    if (data_stack_ptr < 2) {
        printf("2dup -- stack underflow!\n");
        return;
    }
    DCLANG_FLT val2 = data_stack[data_stack_ptr - 1];
    push(data_stack[data_stack_ptr - 2]);
    push(data_stack[data_stack_ptr - 2]);
}

void over2func()
{
    if (data_stack_ptr < 4) {
        printf("2over -- stack underflow!\n");
        return;
    }
    push(data_stack[data_stack_ptr - 4]);
    push(data_stack[data_stack_ptr - 4]);
}

void depthfunc()
{
    DCLANG_PTR size = data_stack_ptr;
    push((DCLANG_FLT)size);
}

void clearfunc()
{
    // clears the stack!
    data_stack_ptr = 0;
}

/////////////////////
// save data stack //
/////////////////////
void svpushfunc()
{
    if (save_data_stack_ptr >= DATA_STACK_SIZE) {
        printf("svpush -- stack overflow!\n");
        save_data_stack_ptr = 0;
    }
    DCLANG_FLT val = dclang_pop();
    data_stack[save_data_stack_ptr++ + DATA_STACK_SIZE] = val;
}

void svpopfunc()
{
    if (save_data_stack_ptr <= 0) {
        printf("svdrop -- stack underflow!\n");
        save_data_stack_ptr = 0;
        return;
    }
    DCLANG_FLT val = data_stack[--save_data_stack_ptr + DATA_STACK_SIZE];
    push_no_check(val);
}

void svdropfunc()
{
    if (save_data_stack_ptr <= 0) {
        printf("svdrop -- stack underflow!\n");
        return;
    }
    --save_data_stack_ptr;
}

void svpickfunc()
{
    if (save_data_stack_ptr <= 0) {
         printf("svpick -- stack underflow!\n");
         return;
    }
    DCLANG_PTR svpick_idx = (DCLANG_PTR) dclang_pop();
    if (save_data_stack_ptr < (svpick_idx + 1)) {
        printf("svpick -- stack not deep enough!\n");
        return;
    }
    push(data_stack[(save_data_stack_ptr - (svpick_idx + 1)) + DATA_STACK_SIZE]);
}

void svdepthfunc()
{
    DCLANG_PTR size = save_data_stack_ptr;
    push((DCLANG_FLT)size);
}

void svclearfunc()
{
    // clears the stack!
    save_data_stack_ptr = 0;
}

////////////
/* output */
////////////
void showfunc()
{
    if (data_stack_ptr < 1) {
        printf(". (pop) -- stack underflow! ");
        return;
    }
    fprintf(ofp, "%0.19g ", dclang_pop());
    fflush(ofp);
}

void shownospacefunc()
{
    if (data_stack_ptr < 1) {
        printf("stack underflow! ");
        return;
    }
    fprintf(ofp, "%0.19g", dclang_pop());
    fflush(ofp);
}

void showstackfunc()
{
    DCLANG_LONG x;
    char *joiner;
    x = data_stack_ptr > 16 ? data_stack_ptr - 16 : 0;
    joiner = x == 0 ? " " : " ... ";
    fprintf(ofp, "data_stack: <%lu>%s", data_stack_ptr, joiner);
    fflush(ofp);
    for (x=0; x < data_stack_ptr; x++) {
        fprintf(ofp, "%0.19g ", data_stack[x]);
        fflush(ofp);
    }
    fprintf(ofp, "\n");
    // do the save data stack as well:
    DCLANG_LONG y;
    char *sv_joiner;
    y = save_data_stack_ptr > 16 ? save_data_stack_ptr - 16 : 0;
    sv_joiner = y == 0 ? " " : " ... ";
    fprintf(ofp, "save_stack: <%lu>%s", save_data_stack_ptr, sv_joiner);
    fflush(ofp);
    for (y=0; y < save_data_stack_ptr; y++) {
        fprintf(ofp, "%0.19g ", data_stack[y + DATA_STACK_SIZE]);
        fflush(ofp);
    }
    fprintf(ofp, "\n");
}

void showrjfunc()
{
    if (data_stack_ptr < 3) {
        printf("Stack underflow! '.rj' needs: value, width, precision on the stack\n");
        return;
    }
    // right-justified for pretty printing!
    int precision = (DCLANG_LONG) dclang_pop();
    int width = (DCLANG_LONG) dclang_pop();
    fprintf(ofp, "%*.*f ", width, precision, dclang_pop());
    fflush(ofp);
}

void showpzfunc()
{
    // left pad with zeros
    if (data_stack_ptr < 3) {
        printf("Stack underflow! '.pz' needs: value, width, precision on the stack\n");
        return;
    }
    int precision = (DCLANG_LONG) dclang_pop();
    int width = (DCLANG_LONG) dclang_pop();
    fprintf(ofp, "%0*.*f ", width, precision, dclang_pop());
    fflush(ofp);
}
/* logical words */
void andfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'and' needs two elements on the stack!\n");
        return;
    }
    push((DCLANG_LONG) dclang_pop() & (DCLANG_LONG) dclang_pop());
}

void orfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'or' needs two elements on the stack!\n");
        return;
    }
    push((DCLANG_LONG) dclang_pop() | (DCLANG_LONG) dclang_pop());
}

void xorfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'xor' needs two elements on the stack!\n");
        return;
    }
    push((DCLANG_LONG) dclang_pop() ^ (DCLANG_LONG) dclang_pop());
}

void notfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'not' needs an element on the stack!\n");
        return;
    }
    push(~(DCLANG_LONG) dclang_pop());
}

/* comparison booleans */
void eqfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'=' needs two elements on the stack!\n");
        return;
    }
    push(((DCLANG_FLT) dclang_pop() == (DCLANG_FLT) dclang_pop()) * -1);
}

void noteqfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'!=' needs two elements on the stack!\n");
        return;
    }
    push(((DCLANG_FLT) dclang_pop() != (DCLANG_FLT) dclang_pop()) * -1);
}

void gtfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'>' needs two elements on the stack!\n");
        return;
    }
    push(((DCLANG_FLT) dclang_pop() < (DCLANG_FLT) dclang_pop()) * -1);
}

void ltfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'<' needs two elements on the stack!\n");
        return;
    }
    push(((DCLANG_FLT) dclang_pop() > (DCLANG_FLT) dclang_pop()) * -1);
}

void gtefunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'>=' needs two elements on the stack!\n");
        return;
    }
    push(((DCLANG_FLT) dclang_pop() <= (DCLANG_FLT) dclang_pop()) * -1);
}

void ltefunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'<=' needs two elements on the stack!\n");
        return;
    }
    push(((DCLANG_FLT) dclang_pop() >= (DCLANG_FLT) dclang_pop()) * -1);
}

// assertions
void assertfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'assert' needs an element on the stack!\n");
        return;
    }
    DCLANG_LONG truth = dclang_pop();
    if (truth == 0) {
        printf("ASSERT FAIL!\n");
    }
}

// true/false syntactic sugar
void truefunc()
{
    push(-1);
}

void falsefunc()
{
    push(0);
}

// null (synonymous with 0)
void nullfunc()
{
    void *ptr = NULL;
    push((DCLANG_LONG)ptr);
}
/* math */
void addfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'+' needs two numbers on the stack!\n");
        return;
    }
    push(dclang_pop() + dclang_pop());
}

void subfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'-' needs two numbers on the stack!\n");
        return;
    }
    DCLANG_FLT subtrahend = dclang_pop();
    push(dclang_pop() - subtrahend);
}

void mulfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'*' needs two numbers on the stack!\n");
        return;
    }
    push(dclang_pop() * dclang_pop());
}

void divfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'/' needs two numbers on the stack!\n");
        return;
    }
    DCLANG_FLT divisor = dclang_pop();
    push(dclang_pop() / divisor);
}

void modfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'%%' needs two numbers on the stack!\n");
        return;
    }
    DCLANG_FLT modulus = dclang_pop();
    push(fmod(dclang_pop(), modulus));
}

void lshiftfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'<<' needs two numbers on the stack!\n");
        return;
    }
    DCLANG_ULONG shift_amt = (DCLANG_ULONG) dclang_pop();
    DCLANG_ULONG base = (DCLANG_ULONG) dclang_pop();
    push((DCLANG_ULONG) base << shift_amt);
}

void rshiftfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'>>' needs two numbers on the stack!\n");
        return;
    }
    DCLANG_ULONG shift_amt = (DCLANG_ULONG) dclang_pop();
    DCLANG_ULONG base = (DCLANG_ULONG) dclang_pop();
    push((DCLANG_ULONG) base >> shift_amt);
}

void absfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'abs' needs a number on the stack!\n");
        return;
    }
    push(fabs(dclang_pop()));
}

void minfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'min' needs two numbers on the stack!\n");
        return;
    }
    DCLANG_FLT a = dclang_pop();
    DCLANG_FLT b = dclang_pop();
    DCLANG_FLT c = (a < b) ? a : b;
    push(c);
}

void maxfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'max' needs two numbers on the stack!\n");
        return;
    }
    DCLANG_FLT a = dclang_pop();
    DCLANG_FLT b = dclang_pop();
    DCLANG_FLT c = (a > b) ? a : b;
    push(c);
}

void roundfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'round' needs a number on the stack!\n");
        return;
    }
    push((DCLANG_LONG) round(dclang_pop()));
}

void floorfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'floor' needs a number on the stack!\n");
        return;
    }
    push((DCLANG_LONG) floor(dclang_pop()));
}

void ceilfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'ceil' needs a number on the stack!\n");
        return;
    }
    push((DCLANG_LONG) ceil(dclang_pop()));
}

/* scientific math words */

void powerfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'pow' needs two numbers on the stack!\n");
        return;
    }
    DCLANG_FLT raise = dclang_pop();
    push(pow(dclang_pop(), raise));
}

void sqrtfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'sqrt' needs a number on the stack!\n");
        return;
    }
    push(sqrt(dclang_pop()));
}

void expfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'exp' need a number on the stack!\n");
        return;
    }
    push(exp(dclang_pop()));
}

void logfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'log' needs a number on the stack!\n");
        return;
    }
    push(log(dclang_pop()));
}

void log2func()
{
    if (data_stack_ptr < 1)
    {
        printf("'log2' needs a number on the stack!\n");
        return;
    }
    push(log2(dclang_pop()));
}

void log10func()
{
    if (data_stack_ptr < 1)
    {
        printf("'log10' needs a number on the stack!\n");
        return;
    }
    push(log10(dclang_pop()));
}

void efunc()
{
    push(M_E);
}

/* Trig, pi, etc. */
void pifunc()
{
    push(M_PI);
}

void sinefunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'sin' needs a number on the stack!\n");
        return;
    }
    push(sin(dclang_pop()));
}

void cosfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'cos' needs a number on the stack!\n");
        return;
    }
    push(cos(dclang_pop()));
}

void tanfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'tan' needs a number on the stack!\n");
        return;
    }
    push(tan(dclang_pop()));
}

void asinefunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'asin' needs a number on the stack!\n");
        return;
    }
    push(asin(dclang_pop()));
}

void acosfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'acos' needs a numbera on the stack!\n");
        return;
    }
    push(acos(dclang_pop()));
}

void atanfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'atan' needs a number on the stack!\n");
        return;
    }
    push(atan(dclang_pop()));
}

void atan2func()
{
    if (data_stack_ptr < 2)
    {
        printf("'atan2' needs two numbers on the stack!\n");
        return;
    }
    DCLANG_FLT x = dclang_pop();
    DCLANG_FLT y = dclang_pop();
    push(atan2(y, x));
}

void sinhfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'sinh' needs a number on the stack!\n");
        return;
    }
    push(sinh(dclang_pop()));
}

void coshfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'cosh' needs a number on the stack!\n");
        return;
    }
    push(cosh(dclang_pop()));
}

void tanhfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'tanh' needs a number on the stack!\n");
        return;
    }
    push(tanh(dclang_pop()));
}

/* randomness */
void randfunc()
{
    push((double)rand()/(double)RAND_MAX);
}
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

void resetoutfunc()
{
    ofp = stdout;
}

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
/* utf-8 char buffer */
char utf8_buf[5];

long utf8_encode(char *out, uint64_t utf)
{
    if (utf <= 0x7F)
    {
        // Plain ASCII
        out[0] = (char) utf;
        out[1] = 0;
        return 1;
    }
    else if (utf <= 0x07FF)
    {
        // 2-byte unicode
        out[0] = (char) (((utf >> 6) & 0x1F) | 0xC0);
        out[1] = (char) (((utf >> 0) & 0x3F) | 0x80);
        out[2] = 0;
        return 2;
    }
    else if (utf <= 0xFFFF)
    {
        // 3-byte unicode
        out[0] = (char) (((utf >> 12) & 0x0F) | 0xE0);
        out[1] = (char) (((utf >>  6) & 0x3F) | 0x80);
        out[2] = (char) (((utf >>  0) & 0x3F) | 0x80);
        out[3] = 0;
        return 3;
    }
    else if (utf <= 0x10FFFF)
    {
        // 4-byte unicode
        out[0] = (char) (((utf >> 18) & 0x07) | 0xF0);
        out[1] = (char) (((utf >> 12) & 0x3F) | 0x80);
        out[2] = (char) (((utf >>  6) & 0x3F) | 0x80);
        out[3] = (char) (((utf >>  0) & 0x3F) | 0x80);
        out[4] = 0;
        return 4;
    }
    else {
        // error - use replacement character
        out[0] = (char) 0xEF;
        out[1] = (char) 0xBF;
        out[2] = (char) 0xBD;
        out[3] = 0;
        return 3;
    }
}

int get_unicode_by_hex(char *chbuf, int usize)
{
    char numstr[usize];
    long int status = (long int) fgets(numstr, usize, ifp);
    int ucode = strtol(numstr, NULL, 16);
    int num_bytes_ret = utf8_encode(chbuf, ucode);
    return status;
}

int get_ascii(char *chbuf, int usize)
{
    char numstr[usize];
    long int status = (long int) fgets(numstr, usize, ifp);
    int acode = strtol(numstr, NULL, 16);
    chbuf[0] = (char) acode;
    chbuf[1] = 0;
    return status;
}

//////////////////////////////////////////////////////////////////////////
// Character emitters. No value goes to the stack; output is immediate. //
//////////////////////////////////////////////////////////////////////////

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
void stringfunc()
{
    char ch;
    char escape_ch;
    char chbuf[5];
    int stat;
    char *scratch = &memory_pool[unused_mem_idx];
    char *start_scratch = scratch;
    // get the next character, and start the process for real:
    if ((ch = fgetc(ifp)) == EOF) exit(0);
    while (! strchr("\"", ch))
    {
        if (strchr("\\", ch))
        {
            /* consume an extra char due to backslash */
            if ((escape_ch = fgetc(ifp)) == EOF) exit(0);
            switch(escape_ch)
            {
                /* backspace */
                case 'b' :
                  chbuf[0] = 8;
                  chbuf[1] = 0;
                  break;
                /* tab */
                case 't' :
                    chbuf[0] = 9;
                    chbuf[1] = 0;
                    break;
                /* newline
                (line-feed and carriage return together on unix)
                */
                case 'n' :
                    chbuf[0] = 10;
                    chbuf[1] = 0;
                    break;
                /* carriage return */
                case 'r' :
                    chbuf[0] = 13;
                    chbuf[1] = 0;
                    break;
                /* 1-byte ASCII code */
                case 'x' :
                    stat = get_ascii(chbuf, 3);
                    if (stat == 0)
                    {
                        printf("Illegal 1-byte ASCII string denoted with \\x.\n");
                        return;
                    }
                    break;
                /* 2-byte unicode */
                case 'u' :
                    stat = get_unicode_by_hex(chbuf, 5);
                    if (stat == 0)
                    {
                        printf("Illegal 2-byte unicode entry in string.\n");
                        return;
                    }
                    break;
                /* 4-byte unicode */
                case 'U' :
                    stat = get_unicode_by_hex(chbuf, 9);
                    if (stat == 0)
                    {
                        printf("Illegal 4-byte unicode entry in string.\n");
                    }
                    break;
                default :
                    chbuf[0] = escape_ch;
                    chbuf[1] = 0;
            }
        } else {
            chbuf[0] = ch;
            chbuf[1] = 0;
        }
        scratch = mempcpy(scratch, chbuf, strlen(chbuf));
        if ((ch = fgetc(ifp)) == EOF) exit(0);
    }
    memset(scratch, 0, 1);
    int chr_cnt = (scratch - start_scratch) + 1;
    unused_mem_idx = (unused_mem_idx + chr_cnt + 0x0f) & ~0x0f;
    // register the string with MIN_STR and MAX_STR
    DCLANG_PTR string_dest_ptr = (DCLANG_PTR) start_scratch;
    DCLANG_PTR buflen = (DCLANG_PTR) chr_cnt;
    if (string_dest_ptr < MIN_STR || MIN_STR == 0)
    {
        MIN_STR = string_dest_ptr;
    }
    if (string_dest_ptr + buflen > MAX_STR || MAX_STR == 0)
    {
        MAX_STR = string_dest_ptr + buflen;
    }
    // Do the right thing depending on def_mode
    if (def_mode)
    {
        prog[iptr].function.with_param = push;
        prog[iptr++].param = string_dest_ptr;
    } else {
        push(string_dest_ptr);
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
// All the logic for reading tokens from stdin and file pointers goes here

void add_to_buf(char ch) {
    if((bufused < IBUFSIZE - 1) && ch != EOF) {
        buf[bufused++] = ch;
    }
}

char *buf2str() {
    buf[bufused++] = '\0';
    return dclang_strdup(buf);
}

void setinput(FILE *fp)  {
    file_stack[fsp++] = ifp;
    ifp = fp;
}

void revertinput() {
    if (fsp == 0) {
        exit(0);
    }
    ifp = file_stack[--fsp];
}

char *get_token() {
    DCLANG_LONG ch;
    bufused = 0;
    // skip leading spaces and comments
    while (1) {
        // skip leading space
        do {
            if((ch = fgetc(ifp)) == EOF) {
                revertinput();
                return "EOF";
            }
        } while(isspace(ch));
        // if we are starting a comment:
        if (strchr("#", ch)) {
            // go to the end of the line
            do {
                if((ch = fgetc(ifp)) == EOF) {
                    revertinput();
                    return "EOF";
                }
            } while(! strchr("\n", ch));
            continue;
        }
        // is this a string?
        if (strchr("\"", ch)) {
            // call the sub-routine to deal with the string:
            stringfunc();
            continue;
        } else {
            add_to_buf(ch);
            break;
        }
    }
    // grab all the next non-whitespace characters
    while (1) {
        // check again for EOF
        if ((ch = fgetc(ifp)) == EOF) {
            revertinput();
            return "EOF";
        }
        if (isspace(ch)) {
            ungetc(ch, ifp);
            return buf2str();
        }
        add_to_buf(ch);
    }
}

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

/* these 'for' loops are more flexible, allowing from/to/step parameters. */
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


/* for debugging */
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

/* respond to ':' token: */
void startword()
{
    /* grab name */
    char *this_token;
    /* TODO: validation */
    this_token = get_token();
    /* put name and current location in user_words lookup array */
    user_words[num_user_words].name = this_token;
    user_words[num_user_words++].word_start = iptr;
}

/* respond to ';' token: */
void endword()
{
    /* Simply insert a return call into 'prog' where 'iptr' now points. */
    prog[iptr++].function.without_param = returnfunc;
    max_iptr = iptr;
}

// deal with setting up local variables

/*
void startlocals()
{
    locals_idx = 0;
    char *new_token = get_token();
    while (strcmp(new_token, "}") != 0) {
        locals_names[locals_idx] = new_token;
        locals_idx += 1;
    }
}

void endlocals()
{

}
*/

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

void pokefunc()
{
    if (data_stack_ptr < 2)
    {
        printf("! -- stack underflow! ");
        return;
    }
    DCLANG_PTR idx = (DCLANG_PTR) dclang_pop();
    if (idx < 0 || idx > NUMVARS)
    {
        printf("! -- variable slot number out-of-range!\n");
        return;
    }
    DCLANG_FLT val = dclang_pop();
    vars[idx] = val;
}

void peekfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("@ -- stack underflow! ");
        return;
    }
    DCLANG_PTR idx = (DCLANG_PTR) dclang_pop();
    if (idx < 0 || idx > NUMVARS)
    {
        printf("@ -- variable slot number %lu is out-of-range!\n", idx);
        return;
    }
    push(vars[idx]);
}

/* implement constants */
void constantfunc()
{
    const_keys[const_idx] = get_token();
    const_vals[const_idx++] = dclang_pop();
}

void variablefunc()
{
    DCLANG_PTR next_var = vars_idx++;
    var_keys[var_map_idx] = get_token();
    var_vals[var_map_idx++] = next_var;
}

void allotfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("allot -- stack underflow! ");
        return;
    }
    vars_idx += (DCLANG_PTR) dclang_pop() - 1;
}

void createfunc()
{
    variablefunc();
    --vars_idx;
}

void commafunc()
{
    if (data_stack_ptr < 1)
    {
        printf(", -- stack underflow! ");
        return;
    }
    DCLANG_FLT val = dclang_pop();
    vars[vars_idx++] = val;
}

void herefunc()
{
    push((DCLANG_PTR) vars_idx);
}

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
    /* grab the key */
    char *key = (const char *)(DCLANG_PTR) dclang_pop();
    DCLANG_PTR key_addr = (DCLANG_PTR) key;
    if (key_addr < MIN_STR || key_addr > MAX_STR)
    {
        perror("h! -- String address for hash key out-of-range.");
        return;
    }
    /* grab the value */
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
    /* grab the key */
    char *key = (char *)(DCLANG_PTR) dclang_pop();
    DCLANG_PTR key_addr = (DCLANG_PTR) key;
    if (key_addr < MIN_STR || key_addr > MAX_STR)
    {
        perror("h@ -- String address for hash key out-of-range.");
        return;
    }
    /* grab the value */
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
    /* grab the key index */
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

void envgetfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("envget -- need <env_key> string on the stack.\n");
    }
    /* grab the key */
    char *env_key = (char *)(DCLANG_PTR) dclang_pop();
    DCLANG_PTR env_key_addr = (DCLANG_PTR) env_key;
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
}

void envsetfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("envset -- need <env_val> <env_key> strings on the stack.\n");
    }
    // grab the key from the stack
    char *env_key = (char *)(DCLANG_PTR) dclang_pop();
    DCLANG_PTR env_key_addr = (DCLANG_PTR) env_key;
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
}

#include "noheap/trees.c"

/* main stuct used for a dictionary-style tree entry.
   'key' is a string, 'value' is a double, which can represent
   either a number or a string pointer.

   It is up to the user to supply checks and use the types
   safely.
*/
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
    /* list device information */
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
void add_primitive(char *name, char *category, void *func_ptr)
{
    primitives_idx += 1;
    (primitives + primitives_idx)->name = name;
    (primitives + primitives_idx)->function = func_ptr;
    (primitives + primitives_idx)->category = category;
};

void add_all_primitives()
{
    primitives = dclang_malloc(208*sizeof(primitive));
    // boolean logic
    add_primitive("null", "Boolean", nullfunc);
    add_primitive("false", "Boolean", falsefunc);
    add_primitive("true", "Boolean", truefunc);
    add_primitive("=", "Boolean", eqfunc);
    add_primitive("<>", "Boolean", noteqfunc);
    add_primitive("<", "Boolean", ltfunc);
    add_primitive(">", "Boolean", gtfunc);
    add_primitive("<=", "Boolean", ltefunc);
    add_primitive(">=", "Boolean", gtefunc);
    add_primitive("assert", "Boolean", assertfunc);
    // bit manipulation
    add_primitive("and", "Bit manipulation", andfunc);
    add_primitive("or", "Bit manipulation", orfunc);
    add_primitive("xor", "Bit manipulation", xorfunc);
    add_primitive("not", "Bit manipulation", notfunc);
    add_primitive("<<", "Bit manipulation", lshiftfunc);
    add_primitive(">>", "Bit manipulation", rshiftfunc);
    // basic arithmetic
    add_primitive("+", "Arithmetic", addfunc);
    add_primitive("-", "Arithmetic", subfunc);
    add_primitive("*", "Arithmetic", mulfunc);
    add_primitive("/", "Arithmetic", divfunc);
    add_primitive("%", "Arithmetic", modfunc);
    add_primitive("abs", "Arithmetic", absfunc);
    add_primitive("min", "Arithmetic", minfunc);
    add_primitive("max", "Arithmetic", maxfunc);
    // float -> int
    add_primitive("round", "Float -> Integer", roundfunc);
    add_primitive("ceil", "Float -> Integer", ceilfunc);
    add_primitive("floor", "Float -> Integer", floorfunc);
    // higher math
    add_primitive("pow", "Higher Math", powerfunc);
    add_primitive("sqrt", "Higher Math", sqrtfunc);
    add_primitive("exp", "Higher Math", expfunc);
    add_primitive("log", "Higher Math", logfunc);
    add_primitive("log2", "Higher Math", log2func);
    add_primitive("log10", "Higher Math", log10func);
    add_primitive("e", "Higher Math", efunc);
    // trig
    add_primitive("pi", "Trigonometry", pifunc);
    add_primitive("sin", "Trigonometry", sinefunc);
    add_primitive("cos", "Trigonometry", cosfunc);
    add_primitive("tan", "Trigonometry", tanfunc);
    add_primitive("asin", "Trigonometry", asinefunc);
    add_primitive("acos", "Trigonometry", acosfunc);
    add_primitive("atan", "Trigonometry", atanfunc);
    add_primitive("atan2", "Trigonometry", atan2func);
    add_primitive("sinh", "Trigonometry", atanfunc);
    add_primitive("cosh", "Trigonometry", atanfunc);
    add_primitive("tanh", "Trigonometry", atanfunc);
    // randomness
    add_primitive("rand", "Randomness", randfunc);
    // stack manipulation
    add_primitive("drop", "Stack Ops", dropfunc);
    add_primitive("dup", "Stack Ops", dupfunc);
    add_primitive("over", "Stack Ops", overfunc);
    add_primitive("pick", "Stack Ops", pickfunc);
    add_primitive("swap", "Stack Ops", swapfunc);
    add_primitive("2drop", "Stack Ops", drop2func);
    add_primitive("2dup", "Stack Ops", dup2func);
    add_primitive("2over", "Stack Ops", over2func);
    add_primitive("depth", "Stack Ops", depthfunc);
    add_primitive("clear", "Stack Ops", clearfunc);
    // the extra "save" stack
    add_primitive("svpush", "Save Stack Ops", svpushfunc);
    add_primitive("svpop", "Save Stack Ops", svpopfunc);
    add_primitive("svdrop", "Save Stack Ops", svdropfunc);
    add_primitive("svpick", "Save Stack Ops", svpickfunc);
    add_primitive("svdepth", "Save Stack Ops", svdepthfunc);
    add_primitive("svclear", "Save Stack Ops", svclearfunc);
    // stack output
    add_primitive(".", "Stack Output", showfunc);
    add_primitive("..", "Stack Output", shownospacefunc);
    add_primitive(".rj", "Stack Output", showrjfunc);
    add_primitive(".pz", "Stack Output", showpzfunc);
    add_primitive(".s", "Stack Output", showstackfunc);
    // variables and arrays
    add_primitive("var", "Variables & Arrays", variablefunc);
    add_primitive("!", "Variables & Arrays", pokefunc);
    add_primitive("@", "Variables & Arrays", peekfunc);
    add_primitive("allot", "Variables & Arrays", allotfunc);
    add_primitive("create", "Variables & Arrays", createfunc);
    add_primitive(",", "Variables & Arrays", commafunc);
    add_primitive("here", "Variables & Arrays", herefunc);
    add_primitive("const", "Variables & Arrays", constantfunc);
    add_primitive("envget", "Variables & Arrays", envgetfunc);
    add_primitive("envset", "Variables & Arrays", envsetfunc);
    // sorting
    add_primitive("sortnums", "Array Sorting", sortnumsfunc);
    add_primitive("sortstrs", "Array Sorting", sortstrsfunc);
    // hash set & get
    add_primitive("h!", "Hashes", hashsetfunc);
    add_primitive("h@", "Hashes", hashgetfunc);
    add_primitive("hkeys", "Hashes", hashkeysfunc);
    // tree storage
    add_primitive("tmake", "Trees", treemakefunc);
    add_primitive("t!", "Trees", treesetfunc);
    add_primitive("t@", "Trees", treegetfunc);
    add_primitive("twalk", "Trees", treewalkfunc);
    add_primitive("tdel", "Trees", treedeletefunc);
#ifdef HAS_TREEDESTROY
    add_primitive("tdestroy", "Trees", treedestroyfunc);
#endif
    // linked lists
    add_primitive("lmake", "Lists", listmakefunc);
    add_primitive("_lnext", "Lists", listnextfunc);
    add_primitive("l!", "Lists", listsetfunc);
    add_primitive("l@", "Lists", listgetfunc);
    add_primitive("lpush", "Lists", listpushfunc);
    add_primitive("lpop", "Lists", listpopfunc);
    add_primitive("lins", "Lists", listinsertfunc);
    add_primitive("lrem", "Lists", listremovefunc);
    add_primitive("lsize", "Lists", listsizefunc);
    add_primitive("ldel", "Lists", listdeletefunc);
    // branching/control
    add_primitive("times", "Branching", timesfunc);
    add_primitive("again", "Branching", againfunc);
    add_primitive("exittimes", "Branching", exittimesfunc);
    add_primitive("for", "Branching", forfunc);
    add_primitive("next", "Branching", nextfunc);
    add_primitive("exitfor", "Branching", exitforfunc);
    add_primitive("i", "Branching", ifunc);
    add_primitive("j", "Branching", jfunc);
    add_primitive("k", "Branching", kfunc);
    add_primitive("if", "Branching", iffunc);
    add_primitive("else", "Branching", elsefunc);
    add_primitive("endif", "Branching", endiffunc);
    add_primitive("return", "Branching", returnfunc);
    // character emitters
    add_primitive("emit", "Character Emitters", emitfunc);
    add_primitive("uemit", "Character Emitters", uemitfunc);
    add_primitive("bytes32", "Character Emitters", bytes32func);
    // character types
    add_primitive("isalnum", "Character Types", isalnumfunc);
    add_primitive("isalpha", "Character Types", isalphafunc);
    add_primitive("iscntrl", "Character Types", iscntrlfunc);
    add_primitive("isdigit", "Character Types", isdigitfunc);
    add_primitive("isgraph", "Character Types", isgraphfunc);
    add_primitive("islower", "Character Types", islowerfunc);
    add_primitive("isprint", "Character Types", isprintfunc);
    add_primitive("ispunct", "Character Types", ispunctfunc);
    add_primitive("isspace", "Character Types", isspacefunc);
    add_primitive("isupper", "Character Types", isupperfunc);
    add_primitive("isxdigit", "Character Types", isxdigitfunc);
    // output and string ops
    add_primitive("cr", "String Output", crfunc);
    add_primitive("print", "String Output", printfunc);
    // string conversion
    add_primitive("tohex", "String Conversion", tohexfunc);
    add_primitive("tostr", "String Conversion", tostrfunc);
    add_primitive("tonum", "String Conversion", tonumfunc);
    add_primitive("ord", "String Conversion", ordfunc);
    add_primitive("tolower", "String Conversion", tolowerfunc);
    add_primitive("toupper", "String Conversion", toupperfunc);
    // general string ops from C
    add_primitive("strlen", "String Ops", strlenfunc);
    add_primitive("str=", "String Ops", streqfunc);
    add_primitive("str<", "String Ops", strltfunc);
    add_primitive("str>", "String Ops", strgtfunc);
    add_primitive("strfind", "String Ops", strfindfunc);
    add_primitive("strspn", "String Ops", strspnfunc);
    add_primitive("strcspn", "String Ops", strcspnfunc);
    add_primitive("strtok", "String Ops", strtokfunc);
    add_primitive("mempcpy", "String Ops", mempcpyfunc);
    add_primitive("memset", "String Ops", memsetfunc);
    // memory buffers
    add_primitive("mkbuf", "Memory", mkbuffunc);
    add_primitive("free", "Memory", freefunc);
    add_primitive("memused", "Memory", memusedfunc);
    // regex
    add_primitive("regcomp", "Regex", regcompfunc);
    add_primitive("regexec", "Regex", regexecfunc);
    add_primitive("regread", "Regex", regreadfunc);
    // file
    add_primitive("fopen", "Files", fileopenfunc);
    add_primitive("fmemopen", "Files", filememopenfunc);
    add_primitive("fread", "Files", filereadfunc);
    add_primitive("freadline", "Files", filereadlinefunc);
    add_primitive("freadall", "Files", filereadallfunc);
    add_primitive("fseek", "Files", fileseekfunc);
    add_primitive("ftell", "Files", filetellfunc);
    add_primitive("fwrite", "Files", filewritefunc);
    add_primitive("fflush", "Files", fileflushfunc);
    add_primitive("fclose", "Files", fileclosefunc);
    add_primitive("redirect", "Files", redirectfunc);
    add_primitive("resetout", "Files", resetoutfunc);
    add_primitive("flush", "Files", flushoutfunc);
    // low-level (OS) file ops:
    add_primitive("open", "Files (no buffer)", openfunc);
    add_primitive("read", "Files (no buffer)", readfunc);
    add_primitive("write", "Files (no buffer)", writefunc);
    add_primitive("close", "Files (no buffer)", closefunc);
    // SQLite3 interface
    add_primitive("_sqlite_open", "SQLite", _sqliteopenfunc);
    add_primitive("_sqlite_prepare", "SQLite", _sqlitepreparefunc);
    add_primitive("_sqlite_step", "SQLite", _sqlitestepfunc);
    add_primitive("_sqlite_column", "SQLite", _sqlitecolumnfunc);
    add_primitive("_sqlite_finalize", "SQLite", _sqlitefinalizefunc);
    add_primitive("_sqlite_exec", "SQLite", _sqliteexecfunc);
    add_primitive("_sqlite_close", "SQLite", _sqliteclosefunc);
    // tcp/udp networking using sockets
    add_primitive("tcplisten", "Sockets", tcplistenfunc);
    add_primitive("tcpaccept", "Sockets", tcpacceptfunc);
    add_primitive("tcpconnect", "Sockets", tcpconnectfunc);
    add_primitive("udprecv", "Sockets", udprecvfunc);
    add_primitive("udpsend", "Sockets", udpsendfunc);
    // time
    add_primitive("clock", "Time", clockfunc);
    add_primitive("sleep", "Time", sleepfunc);
    add_primitive("epoch", "Time", epochfunc);
    add_primitive("dt->epoch", "Time", dt_to_epochfunc);
    add_primitive("epoch->dt", "Time", epoch_to_dtfunc);
    // block a SIGINT
    add_primitive("block_sigint", "Interrupt Blocking", blocksigintfunc);
    add_primitive("unblock_sigint", "Interrupt Blocking", unblocksigintfunc);
    // portmidi
    add_primitive("_pm_list", "MIDI", _pm_listfunc);
    add_primitive("_pm_open_out", "MIDI", _pm_openoutfunc);
    add_primitive("_pm_ws", "MIDI", _pm_wsfunc);
    add_primitive("_pm_wsr", "MIDI", _pm_wsrfunc);
    add_primitive("_pm_close", "MIDI", _pm_closefunc);
    add_primitive("_pm_terminate", "MIDI", _pm_terminatefunc);
    // os fork and exit
    add_primitive("fork", "Operating System", forkfunc);
    add_primitive("exit", "Operating System", exitfunc);
    // show defined words!
    add_primitive("words", "Other", showdefined);
    add_primitive("constants", "Other", showconsts);
    add_primitive("variables", "Other", showvars);
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

    // Search user-defined functions (words)
    DCLANG_LONG found = dclang_findword(token);
    if (found > -1) {
        if (def_mode) {
            if (strcmp(user_words[num_user_words - 1].name, token) == 0) {
                // If the word found is the word immediately being
                // defined, we can avoid `callword`, because that consumes
                // the return stack. Instead, unconditionally jump
                // to the start of the definition.
                prog[iptr].function.with_param = jumpufunc;
                prog[iptr++].param = found;
            } else {
                prog[iptr].function.with_param = callword;
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
                prog[iptr++].function.without_param = pr->function;
            } else {
                if (validate(token)) {
                    (*(pr->function)) ();
                }
            }
            return;
        }
        pr++;
    }

    // Search for a local variable
    if (def_mode) {
        while ((locals_idx < 8) && (locals_keys[locals_idx] != 0)) {
            if (strcmp(locals_keys[locals_idx], token) == 0) {
                prog[iptr].function.with_param = _get_locals_var;
                prog[iptr++].param = locals_idx;
                return;
            }
            // If there's an apostrophe after the variable name, assume it's
            // a directive to set a variable value
            if (
                (strncmp(locals_keys[locals_idx], token, strlen(token)-1) == 0)
                && (strchr(token, '!') != NULL)
            ) {
                prog[iptr].function.with_param = _set_locals_var;
                prog[iptr++].param = locals_idx;
                return;
            }
            locals_idx++;
        }
    }

    // Search for a constant
    while ((const_search_idx >= 0) && (const_keys[const_search_idx] != 0)) {
        if (strcmp(const_keys[const_search_idx], token) == 0) {
            if (def_mode) {
                prog[iptr].function.with_param = push;
                prog[iptr++].param = (DCLANG_FLT) const_vals[const_search_idx];
            } else {
                push((DCLANG_FLT) const_vals[const_search_idx]);
            }
            return;
        }
        const_search_idx--;
    }

    // Search for a variable
    while ((var_search_idx >= 0) && (var_keys[var_search_idx] != 0)) {
        if (strcmp(var_keys[var_search_idx], token) == 0) {
            if (def_mode) {
                prog[iptr].function.with_param = push;
                prog[iptr++].param = (DCLANG_FLT) var_vals[var_search_idx];
            } else {
                push((DCLANG_FLT) var_vals[var_search_idx]);
            }
            return;
        }
        var_search_idx--;
    }

    // Neither user word nor primitive word was found.
    // OK, so next, try to convert to a number
    d = strtod(token, &endPointer);
    if (endPointer != token) {
        if (def_mode) {
            prog[iptr].function.with_param = push;
            prog[iptr++].param = d;
        } else {
            push(d);
        }
        return;
    }

    // Nothing found, we've reached an error condition.
    // Report the situation and reset the stack.
    data_stack_ptr = 0;
    printf("%s -- syntax error.\n", token);
    return;
}


void repl() {
    char *token;
    while (strcmp(token = get_token(), "EOF")) {
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
        // 'compile' it, or interpret it on-the-fly
        compile_or_interpret(token);
    }
    compile_or_interpret(0);
    // return EXIT_SUCCESS;
}


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

// needed so we can add 'import' to primitives
void load_extra_primitives()
{
    add_primitive("primitives", "Other", show_primitivesfunc);
    add_primitive("import", "Other", importfunc);
    add_primitive("input", "Other", inputfunc);
    add_primitive("exec", "Other", execfunc);
    /* final endpoint must be zeros,
       and they won't count in the 'count': */
    add_primitive(0, 0, 0);
}

void dclang_initialize()
{
    setinput(stdin);
    resetoutfunc();
    add_all_primitives();
    load_extra_primitives();
    srand(time(NULL));
    // create the global hash table
    global_hash_table = hcreate(1024*256);
    hashwords = (char**)dclang_malloc(hashwords_size * sizeof(*hashwords));
}
