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

void divmodfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'/%% (divmod)' needs two numbers on the stack!\n");
        return;
    }
    DCLANG_LONG divisor = dclang_pop();
    DCLANG_LONG dividend = dclang_pop();
    ldiv_t result = ldiv(dividend, divisor);
    push(result.rem);
    push(result.quot);
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

////////////////
/* randomness */
////////////////
static uint64_t seed = 1;

void seedfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'seed' needs a seed integer on the stack.\n");
        printf("(A good choice is `clock` or `epoch`).\n");
        return;
    }
    seed = dclang_pop();
}

void randfunc()
{
    /* xorshift64s, variant A_1(12,25,27) with multiplier M_32 from line 3 of table 5 */
    seed ^= seed >> 12;
    seed ^= seed << 25;
    seed ^= seed >> 27;
    push(seed * 0x2545F4914F6CDD1D);
}

//////////////////////////////
// fraction ops (rationals) //
//////////////////////////////

// Euclidean GCD algorithm
DCLANG_INT gcd(DCLANG_INT a, DCLANG_INT b)
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
    DCLANG_UINT  denom = (DCLANG_UINT)  dclang_pop();
    DCLANG_ULONG numer = (DCLANG_ULONG) dclang_pop();
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
    DCLANG_INT numer1 = (DCLANG_INT) (fraction1 >> 32);
    DCLANG_INT numer2 = (DCLANG_INT) (fraction2 >> 32);
    DCLANG_INT denom1 = (DCLANG_INT) fraction1 & 0xffffffff;
    DCLANG_INT denom2 = (DCLANG_INT) fraction2 & 0xffffffff;
    // calculate
    DCLANG_INT result_numer = numer1 * numer2;
    DCLANG_INT result_denom = denom1 * denom2;
    // scale
    DCLANG_INT common = gcd(abs(result_numer), abs(result_denom));
    result_numer = result_numer / common;
    result_denom = result_denom / common;
    // output
    DCLANG_ULONG result_n = (DCLANG_ULONG) result_numer << 32;
    DCLANG_ULONG result_d = (DCLANG_ULONG) result_denom & 0xffffffff;
    push(result_n | result_d);
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
    DCLANG_INT numer1 = (DCLANG_INT) (fraction1 >> 32);
    DCLANG_INT numer2 = (DCLANG_INT) (fraction2 >> 32);
    DCLANG_INT denom1 = (DCLANG_INT) fraction1 & 0xffffffff;
    DCLANG_INT denom2 = (DCLANG_INT) fraction2 & 0xffffffff;
    // calculate
    DCLANG_INT result_numer = numer1 * denom2;
    DCLANG_INT result_denom = denom1 * numer2;
    // scale
    DCLANG_INT common = gcd(abs(result_numer), abs(result_denom));
    result_numer = result_numer / common;
    result_denom = result_denom / common;
    // output
    DCLANG_ULONG result_n = (DCLANG_ULONG) result_numer << 32;
    DCLANG_ULONG result_d = (DCLANG_ULONG) result_denom & 0xffffffff;
    push(result_n | result_d);
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
    DCLANG_INT numer1 = (DCLANG_INT) (fraction1 >> 32);
    DCLANG_INT numer2 = (DCLANG_INT) (fraction2 >> 32);
    DCLANG_INT denom1 = (DCLANG_INT) fraction1 & 0xffffffff;
    DCLANG_INT denom2 = (DCLANG_INT) fraction2 & 0xffffffff;
    // calculate
    numer1 = numer1 * denom2;
    numer2 = numer2 * denom1;
    DCLANG_INT result_numer = numer1 + numer2;
    DCLANG_INT result_denom = denom1 * denom2;
    // scale
    DCLANG_INT common = gcd(abs(result_numer), abs(result_denom));
    result_numer = result_numer / common;
    result_denom = result_denom / common;
    // output
    DCLANG_ULONG result_n = (DCLANG_ULONG) result_numer << 32;
    DCLANG_ULONG result_d = (DCLANG_ULONG) result_denom & 0xffffffff;
    push(result_n | result_d);
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
    DCLANG_INT numer1 = (DCLANG_INT) (fraction1 >> 32);
    DCLANG_INT numer2 = (DCLANG_INT) (fraction2 >> 32);
    DCLANG_INT denom1 = (DCLANG_INT) fraction1 & 0xffffffff;
    DCLANG_INT denom2 = (DCLANG_INT) fraction2 & 0xffffffff;
    // calculate
    numer1 = numer1 * denom2;
    numer2 = numer2 * denom1;
    DCLANG_INT result_numer = numer1 - numer2;
    DCLANG_INT result_denom = denom1 * denom2;
    // scale
    DCLANG_INT common = gcd(abs(result_numer), abs(result_denom));
    result_numer = result_numer / common;
    result_denom = result_denom / common;
    // output
    DCLANG_ULONG result_n = (DCLANG_ULONG) result_numer << 32;
    DCLANG_ULONG result_d = (DCLANG_ULONG) result_denom & 0xffffffff;
    push(result_n | result_d);
}

void fracmediantfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'frmed' (mediant AKA freshman_sum) needs two numbers representing fractions on the stack.\n");
        return;
    }
    // pop fractions
    DCLANG_ULONG fraction2 = dclang_pop();
    DCLANG_ULONG fraction1 = dclang_pop();
    // split numerator and denominator
    DCLANG_INT numer1 = (DCLANG_INT) (fraction1 >> 32);
    DCLANG_INT numer2 = (DCLANG_INT) (fraction2 >> 32);
    DCLANG_INT denom1 = (DCLANG_INT) fraction1 & 0xffffffff;
    DCLANG_INT denom2 = (DCLANG_INT) fraction2 & 0xffffffff;
    // calculate
    DCLANG_INT result_numer = numer1 + numer2;
    DCLANG_INT result_denom = denom1 + denom2;
    // scale
    DCLANG_INT common = gcd(abs(result_numer), abs(result_denom));
    result_numer = result_numer / common;
    result_denom = result_denom / common;
    // output
    DCLANG_ULONG result_n = (DCLANG_ULONG) result_numer << 32;
    DCLANG_ULONG result_d = (DCLANG_ULONG) result_denom & 0xffffffff;
    push(result_n | result_d);
}

void fracsplitfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'frsplit' needs an integer (representing a fraction) on the stack!\n");
        return;
    }
    DCLANG_ULONG fraction = dclang_pop();
    DCLANG_INT numer = (DCLANG_INT) (fraction >> 32);
    DCLANG_INT denom = (DCLANG_INT) (fraction & 0xffffffff);
    push((DCLANG_LONG) numer);
    push((DCLANG_LONG) denom);
}

void showfracfunc()
{
    if (data_stack_ptr < 1)
    {
        printf(".fr needs a number on the stack.\n");
        return;
    }
    DCLANG_ULONG fraction = dclang_pop();
    DCLANG_ULONG numer_l  = fraction >> 32;
    DCLANG_INT numer = (DCLANG_INT) numer_l;
    DCLANG_INT denom = (DCLANG_INT) fraction & 0xffffffff;
    printf("%i/%i ", numer, denom);
}
