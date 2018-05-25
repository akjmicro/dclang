/* This work allows operator lookup to be in constant time.
   As the operator list grows, we may have to change the 'hash_modulus'
   variable and recalculate the operator array lookup indices. */

// basically, djb2 hashing algorithm, reduced by a modulus that accomodates
// any functions we need.
static int get_hash(unsigned char *in_str)
{
  unsigned long int hash = 5381;
  unsigned long int hash_modulus = 367;
  unsigned long int c;
  while ((c = *in_str++)) {
    hash = ((hash << 5) + hash) + c;
  }
  return hash % hash_modulus;
}

// and array of function pointers
void (*operators[367]) ();

// a seperate Python script helped me with populating this. The body of this
// function is generated code.
void populate_operators()
{
  operators[355] = addfunc;
  operators[357] = subfunc;
  operators[354] = mulfunc;
  operators[359] = divfunc;
  operators[349] = modfunc;
  operators[276] = absfunc;
  //operators[225] = lshiftfunc;
  //operators[293] = rshiftfunc;
  operators[6] = eqfunc;
  operators[227] = noteqfunc;
  operators[5] = ltfunc;
  operators[7] = gtfunc;
  operators[226] = ltefunc;
  operators[292] = gtefunc;
  operators[290] = andfunc;
  operators[127] = orfunc;
  operators[183] = notfunc;
  operators[61] = xorfunc;
  operators[151] = pifunc;
  operators[263] = roundfunc;
  operators[351] = ceilfunc;
  operators[209] = floorfunc;
  operators[286] = sinefunc;
  operators[314] = cosfunc;
  operators[10] = tanfunc;
  operators[194] = logfunc;
  operators[213] = log2func;
  operators[162] = powerfunc;
  operators[48] = sqrtfunc;
  operators[1] = dropfunc;
  operators[130] = dupfunc;
  operators[145] = swapfunc;
  operators[40] = overfunc;
  operators[135] = rotfunc;
  operators[348] = nipfunc;
  operators[201] = tuckfunc;
  operators[83] = drop2func;
  operators[303] = dup2func;
  operators[64] = swap2func;
  operators[269] = over2func;
  operators[101] = rot2func;
  operators[128] = dofunc;
  operators[121] = redofunc;
  operators[73] = exitfunc;
  operators[50] = ifunc;
  operators[51] = jfunc;
  operators[52] = kfunc;
  operators[358] = printfunc;
  operators[185] = printstackfunc;
  operators[98] = crfunc;
  operators[108] = clockfunc;
}
