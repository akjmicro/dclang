/* math */
static void addfunc()
{
    push(pop() + pop());
}

static void subfunc()
{
    MYINT subtrahend = pop();
    push(pop() - subtrahend);
}

static void mulfunc()
{
    push(pop() * pop());
}

static void divfunc()
{
    MYINT divisor = pop();
    push(pop() / divisor);
}

static void modfunc()
{
    MYINT modulus = pop();
    push(fmod(pop(), modulus));
}

static void lshiftfunc()
{
    unsigned MYINT shift_amt = (unsigned MYINT) pop();
    unsigned MYINT base = (unsigned MYINT) pop();
    push(base << shift_amt);    
}

static void rshiftfunc()
{
    unsigned MYINT shift_amt = (unsigned MYINT) pop();
    unsigned MYINT base = (unsigned MYINT) pop();
    push(base >> shift_amt);    
}

static void absfunc()
{
    push(fabs(pop()));
}

static void minfunc()
{
    MYINT a = pop();
    MYINT b = pop();
    MYINT c = (a < b) ? a : b;
    push(c);
}

static void maxfunc()
{
    MYINT a = pop();
    MYINT b = pop();
    MYINT c = (a > b) ? a : b;
    push(c);
}
