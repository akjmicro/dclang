/* math */
static void addfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'+' needs two numbers on the stack!\n");
        return;
    }
    push(pop() + pop());
}

static void subfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'-' needs two numbers on the stack!\n");
        return;
    }
    MYFLT subtrahend = pop();
    push(pop() - subtrahend);
}

static void mulfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'*' needs two numbers on the stack!\n");
        return;
    }
    push(pop() * pop());
}

static void divfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'/' needs two numbers on the stack!\n");
        return;
    }
    MYFLT divisor = pop();
    push(pop() / divisor);
}

static void modfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'%%' needs two numbers on the stack!\n");
        return;
    }
    MYFLT modulus = pop();
    push(fmod(pop(), modulus));
}

static void lshiftfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'<<' needs two numbers on the stack!\n");
        return;
    }
    MYUINT shift_amt = (MYUINT) pop();
    MYUINT base = (MYUINT) pop();
    push((MYUINT) base << shift_amt);
}

static void rshiftfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'>>' needs two numbers on the stack!\n");
        return;
    }
    MYUINT shift_amt = (MYUINT) pop();
    MYUINT base = (MYUINT) pop();
    push((MYUINT) base >> shift_amt);
}

static void absfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'abs' needs a number on the stack!\n");
        return;
    }
    push(fabs(pop()));
}

static void minfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'min' needs two numbers on the stack!\n");
        return;
    }
    MYFLT a = pop();
    MYFLT b = pop();
    MYFLT c = (a < b) ? a : b;
    push(c);
}

static void maxfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'max' needs two numbers on the stack!\n");
        return;
    }
    MYFLT a = pop();
    MYFLT b = pop();
    MYFLT c = (a > b) ? a : b;
    push(c);
}

static void roundfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'round' needs a number on the stack!\n");
        return;
    }
    push((MYINT) round(pop()));
}

static void floorfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'floor' needs two numbers on the stack!\n");
        return;
    }
    push((MYINT) floor(pop()));
}

static void ceilfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'ceil' needs two numbers on the stack!\n");
        return;
    }
    push((MYINT) ceil(pop()));
}

/* scientific math words */

static void powerfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'pow' needs two numbers on the stack!\n");
        return;
    }
    MYFLT raise = pop();
    push(pow(pop(), raise));
}

static void sqrtfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'sqrt' needs a number on the stack!\n");
        return;
    }
    push(sqrt(pop()));
}

static void logfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'log' needs a number on the stack!\n");
        return;
    }
    push(log(pop()));
}

static void log2func()
{
    if (data_stack_ptr < 1)
    {
        printf("'log2' needs a number on the stack!\n");
        return;
    }
    push(log2(pop()));
}

static void log10func()
{
    if (data_stack_ptr < 1)
    {
        printf("'log10' needs a number on the stack!\n");
        return;
    }
    push(log10(pop()));
}

static void efunc()
{
    push(M_E);
}

/* Trig, pi, etc. */
static void pifunc()
{
    push(M_PI);
}

static void sinefunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'sin' needs two numbers on the stack!\n");
        return;
    }
    push(sin(pop()));
}

static void cosfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'cos' needs two numbers on the stack!\n");
        return;
    }
    push(cos(pop()));
}

static void tanfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'tan' needs two numbers on the stack!\n");
        return;
    }
    push(tan(pop()));
}

/* randomness */
static void randfunc()
{
    push((double)rand()/(double)RAND_MAX);
}
