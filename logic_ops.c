/* logical words */
void andfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'and' needs two elements on the stack!\n");
        return;
    }
    push((DCLANG_INT) dclang_pop() & (DCLANG_INT) dclang_pop());
}

void orfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'or' needs two elements on the stack!\n");
        return;
    }
    push((DCLANG_INT) dclang_pop() | (DCLANG_INT) dclang_pop());
}

void xorfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'xor' needs two elements on the stack!\n");
        return;
    }
    push((DCLANG_INT) dclang_pop() ^ (DCLANG_INT) dclang_pop());
}

void notfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'not' needs an element on the stack!\n");
        return;
    }
    push(~(DCLANG_INT) dclang_pop());
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
    DCLANG_INT truth = dclang_pop();
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
    push((DCLANG_INT)ptr);
}
