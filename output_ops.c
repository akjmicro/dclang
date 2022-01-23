/* output */
static void showfunc()
{
    if (data_stack_ptr < 1) {
        printf(". (pop) -- stack underflow! ");
        return;
    }
    fprintf(ofp, "%0.19g ", pop());
    fflush(ofp);
}

static void shownospacefunc()
{
    if (data_stack_ptr < 1) {
        printf("stack underflow! ");
        return;
    }
    fprintf(ofp, "%0.19g", pop());
    fflush(ofp);
}

static void crfunc()
{
    fprintf(ofp, "\n");
}

static void showstackfunc()
{
    DCLANG_INT x;
    char *joiner;
    x = data_stack_ptr > 16 ? data_stack_ptr - 16 : 0;
    joiner = x == 0 ? " " : " ... ";
    fprintf(ofp, "data_stack: <%i>%s", data_stack_ptr, joiner);
    fflush(ofp);
    for (x=0; x < data_stack_ptr; x++) {
        fprintf(ofp, "%0.19g ", data_stack[x]);
        fflush(ofp);
    }
    fprintf(ofp, "\n");
    // do the save data stack as well:
    DCLANG_INT y;
    char *sv_joiner;
    y = save_data_stack_ptr > 16 ? save_data_stack_ptr - 16 : 0;
    sv_joiner = y == 0 ? " " : " ... ";
    fprintf(ofp, "save_stack: <%i>%s", save_data_stack_ptr, sv_joiner);
    fflush(ofp);
    for (y=0; y < save_data_stack_ptr; y++) {
        fprintf(ofp, "%0.19g ", save_data_stack[y]);
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
    int precision = (DCLANG_INT) pop();
    int width = (DCLANG_INT) pop();
    fprintf(ofp, "%*.*g ", width, precision, pop());
    fflush(ofp);
}

static void redirectfunc()
{
    if (data_stack_ptr < 1) {
        printf("Stack underflow! 'redirect' needs an output file pointer before being called\n");
        return;
    }
    ofp = (FILE *)(DCLANG_UINT) pop();
}

static void resetoutfunc()
{
    ofp = stdout;
}

static void flushoutfunc()
{
    fflush(ofp);
}
