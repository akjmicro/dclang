/* math */
static void addfunc()
{
    push(pop() + pop());
}

static void subfunc()
{
    long double subtrahend = pop();
    push(pop() - subtrahend);
}

static void mulfunc()
{
    push(pop() * pop());
}

static void divfunc()
{
    long double divisor = pop();
    push(pop() / divisor);
}

static void modfunc()
{
    long double modulus = pop();
    push(fmod(pop(), modulus));
}

static void lshiftfunc()
{
    unsigned int shift_amt = (unsigned int) pop();
    unsigned int base = (unsigned int) pop();
    push(base << shift_amt);    
}

static void rshiftfunc()
{
    unsigned int shift_amt = (unsigned int) pop();
    unsigned int base = (unsigned int) pop();
    push(base >> shift_amt);    
}

static void absfunc()
{
    push(fabs(pop()));
}

static void roundfunc()
{
    push((long int) round(pop()));
}

static void floorfunc()
{
    push((long int) floor(pop()));
}

static void ceilfunc()
{
    push((long int) ceil(pop()));
}

/* scientific math words */

static void powerfunc()
{
    long double raise = pop();
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
