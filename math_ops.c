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
    DCLANG_INT subtrahend = dclang_pop();
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
    DCLANG_INT divisor = dclang_pop();
    push(dclang_pop() / divisor);
}

void modfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'%%' needs two numbers on the stack!\n");
        return;
    }
    DCLANG_INT modulus = dclang_pop();
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
    DCLANG_INT a = dclang_pop();
    DCLANG_INT b = dclang_pop();
    DCLANG_INT c = (a < b) ? a : b;
    push(c);
}

void maxfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'max' needs two numbers on the stack!\n");
        return;
    }
    DCLANG_INT a = dclang_pop();
    DCLANG_INT b = dclang_pop();
    DCLANG_INT c = (a > b) ? a : b;
    push(c);
}

/* randomness */
/* xorshift64s, variant A_1(12,25,27) with multiplier M_32 from line 3 of table 5 */
void randfunc() {
    /* initial seed must be nonzero, don't use a static variable for the state if multithreaded */
    static uint64_t x = 1;
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    push(x * 0x2545F4914F6CDD1D);
}
