/* output */
static void showfunc()
{
    if (data_stack_ptr < 1) {
        printf("stack underflow! ");
        return;
    }
    fprintf(ofp, "%0.16g ", pop());
}

static void shownospacefunc()
{
    if (data_stack_ptr < 1) {
        printf("stack underflow! ");
        return;
    }
    fprintf(ofp, "%0.16g", pop());
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
        fprintf(ofp, "%0.16g ", data_stack[x]);
        fflush(ofp);
    }
    fprintf(ofp, "\n");
}

static void showrjfunc()
{
    if (data_stack_ptr < 3) {
        printf("Stack underflow! '.rj' needs: value, width, precision on the stack\n");
        return;
    }
    // right-justified for pretty printing!
    int precision = (MYINT) pop();
    int width = (MYINT) pop();
    fprintf(ofp, "%*.*g ", width, precision, pop());
}

static void redirectfunc()
{
    if (data_stack_ptr < 1) {
        printf("Stack underflow! 'redirect needs an output file pointer before being called\n");
        return;
    }
    ofp = (FILE *)(unsigned long int) pop();
}

static void resetoutfunc()
{
    ofp = stdout;
}
