/* This work allows operator lookup to be in constant time.
   As the operator list grows, we may have to change the 'hash_modulus'
   variable and recalculate the operator array lookup indices. */

// basically, djb2 hashing algorithm, reduced by a modulus that accomodates
// any functions we need. This is probably going to be deprecated, as far as 
// function lookup is concerned.
/*static int get_hash(unsigned char *in_str)
{
  unsigned long int hash = 5381;
  unsigned long int hash_modulus = 367;
  unsigned long int c;
  while ((c = *in_str++)) {
    hash = ((hash << 5) + hash) + c;
  }
  return hash % hash_modulus;
}*/

/* we need to populate the dictionary first with primitives */
struct primitive {
  const char *name;
  void (*function) (void);
};

static const struct primitive primitives[] = {
  {"+", addfunc},
  {"-", subfunc},
  {"*", mulfunc},
  {"/", divfunc},
  {"%", modfunc},
  {"abs", absfunc},
  {"<<", lshiftfunc},
  {">>", rshiftfunc},
  {"=", eqfunc},
  {"<>", noteqfunc},
  {"<", ltfunc},
  {">", gtfunc},
  {"<=", ltefunc},
  {">=", gtefunc},
  {"and", andfunc},
  {"or", orfunc},
  {"not", notfunc},
  {"xor", xorfunc},
  {"pi", pifunc},
  {"e", efunc},
  {"round", roundfunc},
  {"ceil", ceilfunc},
  {"floor", floorfunc},
  {"log", logfunc},
  {"log2", log2func},
  {"pow", powerfunc},
  {"sin", sinefunc},
  {"cos", cosfunc},
  {"tan", tanfunc},
  {"sqrt", sqrtfunc},
  {"drop", dropfunc},
  {"dup", dupfunc},
  {"swap", swapfunc},
  {"over", overfunc},
  {"rot", rotfunc},
  {"nip", nipfunc},
  {"tuck", tuckfunc},
  {"2drop", drop2func},
  {"2dup", dup2func},
  {"2swap", swap2func},
  {"2over", over2func},
  {"2rot", rot2func},
  {"!", pokefunc},
  {"@", peekfunc},
  {"do", dofunc},
  {"redo", redofunc},
  {"exit", exitfunc},
  {"skip", skipfunc},
  {"i", ifunc},
  {"j", jfunc},
  {".", showfunc},
  {".s", showstackfunc},
  {".rj", showrjfunc},
  {"cr", crfunc},
  {"s\"", stringfunc},
  {"print", printfunc},
  {"clock", clockfunc},
  {"rdtsc", rdtscfunc},
  {0, 0}
};
