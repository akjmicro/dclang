/* math */
void addfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'+' needs two numbers on the stack!\n");
        return;
    }
    push(dclang_pop() + dclang_pop());
}

void subfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'-' needs two numbers on the stack!\n");
        return;
    }
    DCLANG_FLT subtrahend = dclang_pop();
    push(dclang_pop() - subtrahend);
}

void mulfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'*' needs two numbers on the stack!\n");
        return;
    }
    push(dclang_pop() * dclang_pop());
}

void divfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'/' needs two numbers on the stack!\n");
        return;
    }
    DCLANG_FLT divisor = dclang_pop();
    push(dclang_pop() / divisor);
}

void modfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'%%' needs two numbers on the stack!\n");
        return;
    }
    DCLANG_FLT modulus = dclang_pop();
    push(fmod(dclang_pop(), modulus));
}

void lshiftfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'<<' needs two numbers on the stack!\n");
        return;
    }
    DCLANG_UINT shift_amt = (DCLANG_UINT) dclang_pop();
    DCLANG_UINT base = (DCLANG_UINT) dclang_pop();
    push((DCLANG_UINT) base << shift_amt);
}

void rshiftfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'>>' needs two numbers on the stack!\n");
        return;
    }
    DCLANG_UINT shift_amt = (DCLANG_UINT) dclang_pop();
    DCLANG_UINT base = (DCLANG_UINT) dclang_pop();
    push((DCLANG_UINT) base >> shift_amt);
}

void absfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'abs' needs a number on the stack!\n");
        return;
    }
    push(fabs(dclang_pop()));
}

void minfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'min' needs two numbers on the stack!\n");
        return;
    }
    DCLANG_FLT a = dclang_pop();
    DCLANG_FLT b = dclang_pop();
    DCLANG_FLT c = (a < b) ? a : b;
    push(c);
}

void maxfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'max' needs two numbers on the stack!\n");
        return;
    }
    DCLANG_FLT a = dclang_pop();
    DCLANG_FLT b = dclang_pop();
    DCLANG_FLT c = (a > b) ? a : b;
    push(c);
}

void roundfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'round' needs a number on the stack!\n");
        return;
    }
    push((DCLANG_INT) round(dclang_pop()));
}

void floorfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'floor' needs a number on the stack!\n");
        return;
    }
    push((DCLANG_INT) floor(dclang_pop()));
}

void ceilfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'ceil' needs a number on the stack!\n");
        return;
    }
    push((DCLANG_INT) ceil(dclang_pop()));
}

/* scientific math words */

void powerfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'pow' needs two numbers on the stack!\n");
        return;
    }
    DCLANG_FLT raise = dclang_pop();
    push(pow(dclang_pop(), raise));
}

void sqrtfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'sqrt' needs a number on the stack!\n");
        return;
    }
    push(sqrt(dclang_pop()));
}

void expfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'exp' need a number on the stack!\n");
        return;
    }
    push(exp(dclang_pop()));
}

void logfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'log' needs a number on the stack!\n");
        return;
    }
    push(log(dclang_pop()));
}

void log2func()
{
    if (data_stack_ptr < 1)
    {
        printf("'log2' needs a number on the stack!\n");
        return;
    }
    push(log2(dclang_pop()));
}

void log10func()
{
    if (data_stack_ptr < 1)
    {
        printf("'log10' needs a number on the stack!\n");
        return;
    }
    push(log10(dclang_pop()));
}

void efunc()
{
    push(M_E);
}

/* Trig, pi, etc. */
void pifunc()
{
    push(M_PI);
}

void sinefunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'sin' needs a number on the stack!\n");
        return;
    }
    push(sin(dclang_pop()));
}

void cosfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'cos' needs a number on the stack!\n");
        return;
    }
    push(cos(dclang_pop()));
}

void tanfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'tan' needs a number on the stack!\n");
        return;
    }
    push(tan(dclang_pop()));
}

void asinefunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'asin' needs a number on the stack!\n");
        return;
    }
    push(asin(dclang_pop()));
}

void acosfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'acos' needs a numbera on the stack!\n");
        return;
    }
    push(acos(dclang_pop()));
}

void atanfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'atan' needs a number on the stack!\n");
        return;
    }
    push(atan(dclang_pop()));
}

void atan2func()
{
    if (data_stack_ptr < 2)
    {
        printf("'atan2' needs two numbers on the stack!\n");
        return;
    }
    DCLANG_FLT x = dclang_pop();
    DCLANG_FLT y = dclang_pop();
    push(atan2(y, x));
}

void sinhfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'sinh' needs a number on the stack!\n");
        return;
    }
    push(sinh(dclang_pop()));
}

void coshfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'cosh' needs a number on the stack!\n");
        return;
    }
    push(cosh(dclang_pop()));
}

void tanhfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'tanh' needs a number on the stack!\n");
        return;
    }
    push(tanh(dclang_pop()));
}

/* randomness */
void randfunc()
{
    push((double)rand()/(double)RAND_MAX);
}
