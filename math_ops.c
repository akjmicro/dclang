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
    DCLANG_LONG subtrahend = dclang_pop();
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
    DCLANG_LONG divisor = dclang_pop();
    push(dclang_pop() / divisor);
}

void modfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'%%' needs two numbers on the stack!\n");
        return;
    }
    DCLANG_LONG modulus = dclang_pop();
    push(fmod(dclang_pop(), modulus));
}

void lshiftfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'<<' needs two numbers on the stack!\n");
        return;
    }
    DCLANG_ULONG shift_amt = (DCLANG_ULONG) dclang_pop();
    DCLANG_ULONG base = (DCLANG_ULONG) dclang_pop();
    push((DCLANG_ULONG) base << shift_amt);
}

void rshiftfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'>>' needs two numbers on the stack!\n");
        return;
    }
    DCLANG_ULONG shift_amt = (DCLANG_ULONG) dclang_pop();
    DCLANG_ULONG base = (DCLANG_ULONG) dclang_pop();
    push((DCLANG_ULONG) base >> shift_amt);
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
    DCLANG_LONG a = dclang_pop();
    DCLANG_LONG b = dclang_pop();
    DCLANG_LONG c = (a < b) ? a : b;
    push(c);
}

void maxfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'max' needs two numbers on the stack!\n");
        return;
    }
    DCLANG_LONG a = dclang_pop();
    DCLANG_LONG b = dclang_pop();
    DCLANG_LONG c = (a > b) ? a : b;
    push(c);
}

/* randomness */
/* xorshift64s, variant A_1(12,25,27) with multiplier M_32 from line 3 of table 5 */
void randfunc()
{
    /* initial seed must be nonzero, don't use a static variable for the state if multithreaded */
    static uint64_t x = 1;
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    push(x * 0x2545F4914F6CDD1D);
}

//////////////////////////////
// fraction ops (rationals) //
//////////////////////////////

// Euclidean GCD algorithm
DCLANG_LONG gcd(DCLANG_LONG a, DCLANG_LONG b)
{
    if (abs(b - a) == 1) return 1;
    int result = a % b;
    while (result != 0) {
        a = b;
        b = result;
        result = a % b;
    }
    return b;
}

void fracfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'frac' needs two integers on the stack!\n");
        return;
    }
    DCLANG_UINT  denom    = (DCLANG_UINT)  dclang_pop();
    DCLANG_ULONG numer    = (DCLANG_ULONG) dclang_pop();
    DCLANG_ULONG fraction = (numer << 32) | denom;
    push(fraction);
}

void fracmulfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'fr*' needs two numbers on the stack!\n");
        return;
    }
    // pop fractions
    DCLANG_ULONG fraction2 = dclang_pop();
    DCLANG_ULONG fraction1 = dclang_pop();
    // split numerator and denominator
    DCLANG_LONG numer1 = fraction1 >> 32;
    DCLANG_LONG numer2 = fraction2 >> 32;
    DCLANG_INT  denom1 = (DCLANG_INT) fraction1 & 0xffffffff;
    DCLANG_INT  denom2 = (DCLANG_INT) fraction2 & 0xffffffff;
    // calculate
    DCLANG_LONG result_numer = numer1 * numer2;
    DCLANG_LONG result_denom = denom1 * denom2;
    DCLANG_LONG common = gcd(result_numer, result_denom);
    result_numer = (result_numer / common) << 32;
    result_denom = result_denom / common;
    push(result_numer | result_denom);
}

void fracdivfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'fr/' needs two numbers on the stack!\n");
        return;
    }
    // pop fractions
    DCLANG_ULONG fraction2 = dclang_pop();
    DCLANG_ULONG fraction1 = dclang_pop();
    // split numerator and denominator
    DCLANG_LONG numer1 = fraction1 >> 32;
    DCLANG_LONG numer2 = fraction2 >> 32;
    DCLANG_INT  denom1 = (DCLANG_INT) fraction1 & 0xffffffff;
    DCLANG_INT  denom2 = (DCLANG_INT) fraction2 & 0xffffffff;
    // calculate
    DCLANG_LONG result_numer = numer1 * denom2;
    DCLANG_LONG result_denom = denom1 * numer2;
    DCLANG_LONG common = gcd(result_numer, result_denom);
    result_numer = (result_numer / common) << 32;
    result_denom = result_denom / common;
    push(result_numer | result_denom);
}

void fracaddfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'fr+' needs two numbers on the stack!\n");
        return;
    }
    // pop fractions
    DCLANG_ULONG fraction2 = dclang_pop();
    DCLANG_ULONG fraction1 = dclang_pop();
    // split numerator and denominator
    DCLANG_LONG numer1 = fraction1 >> 32;
    DCLANG_LONG numer2 = fraction2 >> 32;
    DCLANG_INT  denom1 = (DCLANG_INT) fraction1 & 0xffffffff;
    DCLANG_INT  denom2 = (DCLANG_INT) fraction2 & 0xffffffff;
    // calculate
    numer1 = numer1 * denom2;
    numer2 = numer2 * denom1;
    DCLANG_LONG result_numer = numer1 + numer2;
    DCLANG_LONG result_denom = denom1 * denom2;
    DCLANG_LONG common = gcd(result_numer, result_denom);
    result_numer = (result_numer / common) << 32;
    result_denom = result_denom / common;
    push(result_numer | result_denom);
}

void fracsubfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'fr-' needs two numbers on the stack!\n");
        return;
    }
    // pop fractions
    DCLANG_ULONG fraction2 = dclang_pop();
    DCLANG_ULONG fraction1 = dclang_pop();
    // split numerator and denominator
    DCLANG_LONG numer1 = fraction1 >> 32;
    DCLANG_LONG numer2 = fraction2 >> 32;
    DCLANG_INT  denom1 = (DCLANG_INT) fraction1 & 0xffffffff;
    DCLANG_INT  denom2 = (DCLANG_INT) fraction2 & 0xffffffff;
    // calculate
    numer1 = numer1 * denom2;
    numer2 = numer2 * denom1;
    DCLANG_LONG result_numer = numer1 - numer2;
    DCLANG_LONG result_denom = denom1 * denom2;
    DCLANG_LONG common = gcd(result_numer, result_denom);
    result_numer = (result_numer / common) << 32;
    result_denom = result_denom / common;
    push(result_numer | result_denom);
}

void showfracfunc()
{
    if (data_stack_ptr < 1)
    {
        printf(".fr needs a number on the stack.\n");
        return;
    }
    DCLANG_ULONG fraction = dclang_pop();
    DCLANG_LONG  numer    = fraction >> 32;
    DCLANG_INT   denom    = (DCLANG_INT) fraction & 0xffffffff;
    printf("%0.0i/%0.0i ", numer, denom);
}
