/* logical words */
static void andfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'and' needs two elements on the stack!\n");
        return;
    }
    push((MYINT) pop() & (MYINT) pop());
}

static void orfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'or' needs two elements on the stack!\n");
        return;
    }
    push((MYINT) pop() | (MYINT) pop());
}

static void xorfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'xor' needs two elements on the stack!\n");
        return;
    }
    push((MYINT) pop() ^ (MYINT) pop());
}

static void notfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'not' needs an element on the stack!\n");
        return;
    }
    push(~(MYINT) pop());
}

/* comparison booleans */
static void eqfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'=' needs two elements on the stack!\n");
        return;
    }
    push((MYFLT) pop() == (MYFLT) pop());
}

static void noteqfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'!=' needs two elements on the stack!\n");
        return;
    }
    push((MYFLT) pop() != (MYFLT) pop());
}

static void gtfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'>' needs two elements on the stack!\n");
        return;
    }
    push((MYFLT) pop() < (MYFLT) pop());
}

static void ltfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'<' needs two elements on the stack!\n");
        return;
    }
    push((MYFLT) pop() > (MYFLT) pop());
}

static void gtefunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'>=' needs two elements on the stack!\n");
        return;
    }
    push((MYFLT) pop() <= (MYFLT) pop());
}

static void ltefunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'<=' needs two elements on the stack!\n");
        return;
    }
    push((MYFLT) pop() >= (MYFLT) pop());
}

// assertions
static void assertfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'assert' needs an element on the stack!\n");
        return;
    }
    MYINT truth = pop();
    if (truth == 0) {
        printf("ASSERT FAIL!\n");
    }
}

// true/false syntactic sugar
static void truefunc()
{
    push(-1);
}

static void falsefunc()
{
    push(0);
}
