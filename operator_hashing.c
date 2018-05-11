/* This work allows operator lookup to be in constant time.
   As the operator list grows, we may have to change the 'hash_modulus'
   variable and recalculate the operator array lookup indices. */

// basically, a reduced (by modulus) djb2 hashing algorithm.
static int get_hash(unsigned char *in_str)
{
  unsigned long int hash = 5381;
  unsigned long int hash_modulus = 586;
  unsigned long int c;
  while ((c = *in_str++)) {
    hash = ((hash << 5) + hash) + c;
  }
  return hash % hash_modulus;
}

// and array of function pointers
void (*operators[586]) ();

// a seperate Python script helped me with populating this. The body of this
// function is generated code.
void populate_operators()
{
  operators[58] = addfunc;
  operators[60] = subfunc;
  operators[57] = mulfunc;
  operators[62] = divfunc;
  operators[52] = modfunc;
  operators[499] = absfunc;
  operators[76] = eqfunc;
  operators[193] = noteqfunc;
  operators[75] = ltfunc;
  operators[77] = gtfunc;
  operators[192] = ltefunc;
  operators[258] = gtefunc;
  operators[294] = andfunc;
  operators[170] = orfunc;
  operators[436] = notfunc;
  operators[190] = xorfunc;
  operators[194] = pifunc;
  operators[403] = sinefunc;
  operators[176] = cosfunc;
  operators[56] = tanfunc;
  operators[3] = logfunc;
  operators[149] = log2func;
  operators[273] = powerfunc;
  operators[577] = sqrtfunc;
  operators[456] = dropfunc;
  operators[288] = dupfunc;
  operators[100] = swapfunc;
  operators[139] = overfunc;
  operators[104] = rotfunc;
  operators[234] = nipfunc;
  operators[518] = tuckfunc;
  operators[16] = drop2func;
  operators[470] = dup2func;
  operators[246] = swap2func;
  operators[285] = over2func;
  operators[502] = beginfunc;
  operators[353] = againfunc;
  operators[541] = exitfunc;
  operators[120] = ifunc;
  operators[121] = jfunc;
  operators[122] = kfunc;
  operators[61] = printfunc;
  operators[370] = printstackfunc;
  operators[360] = crfunc;
  operators[245] = clockfunc;
}
