/* output */
static void showfunc()
{
    if (data_stack_ptr < 1) {
        printf("stack underflow! ");
        return;
    }
    fprintf(ofp, "%li ", pop());
    fflush(ofp);
}

static void shownospacefunc()
{
    if (data_stack_ptr < 1) {
        printf("stack underflow! ");
        return;
    }
    fprintf(ofp, "%li", pop());
    fflush(ofp);
}

static void showunsignedfunc()
{
    if (data_stack_ptr < 1) {
        printf("stack underflow! ");
        return;
    }
    fprintf(ofp, "%lu ", pop());
    fflush(ofp);
}

static void showhexfunc()
{
    if (data_stack_ptr < 1) {
        printf("stack underflow! ");
        return;
    }
    fprintf(ofp, "0x%x ", pop());
    fflush(ofp);
}

static void crfunc()
{
    fprintf(ofp, "\n");
}

static void showstackfunc()
{
    MYINT x;
    char *joiner;
    x = data_stack_ptr > 16 ? data_stack_ptr - 16 : 0;
    joiner = x == 0 ? " " : " ... "; 
    fprintf(ofp, "<%i>%s", data_stack_ptr, joiner);
    fflush(ofp);
    for (x=0; x < data_stack_ptr; x++) {
        fprintf(ofp, "%li ", data_stack[x]);
    }
    fprintf(ofp, "\n");
}

static void showunsignedstackfunc()
{
    MYINT x;
    char *joiner;
    x = data_stack_ptr > 16 ? data_stack_ptr - 16 : 0;
    joiner = x == 0 ? " " : " ... ";
    fprintf(ofp, "<%lu>%s", data_stack_ptr, joiner);
    for (x=0; x < data_stack_ptr; x++) {
        fprintf(ofp, "%lu ", data_stack[x]);
    }
    fprintf(ofp, "\n");
}

static void showhexstackfunc()
{
    MYINT x;
    char *joiner;
    x = data_stack_ptr > 16 ? data_stack_ptr - 16 : 0;
    joiner = x == 0 ? " " : " ... ";
    printf("<0x%x>%s", data_stack_ptr, joiner);
    for (x=0; x < data_stack_ptr; x++) {
        fprintf(ofp, "0x%x ", data_stack[x]);
    }
    fprintf(ofp, "\n");
}

static void redirectfunc()
{
    if (data_stack_ptr < 1) {
        printf("Stack underflow! 'redirect' needs an output file pointer before being called\n");
        return;
    }
    ofp = (FILE *)(unsigned long int) pop();
}

static void resetoutfunc()
{
    ofp = stdout;
}
