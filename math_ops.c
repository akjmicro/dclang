/* math */
static void addfunc()
{
    push(pop() + pop());
}

static void subfunc()
{
    MYFLT subtrahend = pop();
    push(pop() - subtrahend);
}

static void mulfunc()
{
    push(pop() * pop());
}

static void divfunc()
{
    MYFLT divisor = pop();
    push(pop() / divisor);
}

static void modfunc()
{
    MYFLT modulus = pop();
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

static void roundfunc()
{
    push((MYINT) round(pop()));
}

static void floorfunc()
{
    push((MYINT) floor(pop()));
}

static void ceilfunc()
{
    push((MYINT) ceil(pop()));
}

/* scientific math words */

static void powerfunc()
{
    MYFLT raise = pop();
    push(pow(pop(), raise));
}

static void sqrtfunc()
{
    push(sqrt(pop()));
}

static void logfunc()
{
    push(log(pop()));
}

static void log2func()
{
    push(log2(pop())); 
}

static void log10func()
{
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
    push(sin(pop()));
}

static void cosfunc()
{
    push(cos(pop()));
}

static void tanfunc()
{
    push(tan(pop()));
}

/* randomness */
static void randfunc()
{
    push((double)rand()/(double)RAND_MAX);
}
