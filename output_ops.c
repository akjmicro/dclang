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
