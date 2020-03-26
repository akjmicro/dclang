/* we need to populate the dictionary first with primitives */
struct primitive {
  const char *name;
  void (*function) (void);
};

static struct primitive primitives[118] = {
  // null
  {"null", nullfunc},
  {"isnull", isnullfunc},
  // basic arithmetic
  {"+", addfunc},
  {"-", subfunc},
  {"*", mulfunc},
  {"/", divfunc},
  {"%", modfunc},
  {"abs", absfunc},
  {"min", minfunc},
  {"max", maxfunc},
  {"<<", lshiftfunc},
  {">>", rshiftfunc},
  // comparisons
  {"=", eqfunc},
  {"<>", noteqfunc},
  {"<", ltfunc},
  {">", gtfunc},
  {"<=", ltefunc},
  {">=", gtefunc},
  {"assert", assertfunc},
  // logic
  {"true", truefunc},
  {"false", falsefunc},
  {"and", andfunc},
  {"or", orfunc},
  {"not", notfunc},
  {"xor", xorfunc},
  // float -> int
  {"round", roundfunc},
  {"ceil", ceilfunc},
  {"floor", floorfunc},
  // higher math
  {"pow", powerfunc},
  {"sqrt", sqrtfunc},
  {"log", logfunc},
  {"log2", log2func},
  {"log10", log10func},
  // constants
  {"pi", pifunc},
  {"e", efunc},
  // trig
  {"sin", sinefunc},
  {"cos", cosfunc},
  {"tan", tanfunc},
  // randomness
  {"rand", randfunc},
  // stack operators
  {"clear", clearfunc},
  {"drop", dropfunc},
  {"dup", dupfunc},
  {"over", overfunc},
  {"pick", pickfunc},
  {"swap", swapfunc},
  {"rot", rotfunc},
  {"-rot", rotnegfunc},
  {"nip", nipfunc},
  {"tuck", tuckfunc},
  {"2drop", drop2func},
  {"2dup", dup2func},
  {"2over", over2func},
  {"2swap", swap2func},
  {"2rot", rot2func},
  {"-2rot", rotneg2func},
  {"2nip", nip2func},
  {"2tuck", tuck2func},
  // extra stack
  {"savepush", savepush},
  {"savepop", savepop},
  // branching
  {"times", timesfunc},
  {"again", againfunc},
  {"exittimes", exittimesfunc},
  {"for", forfunc},
  {"next", nextfunc},
  {"exitfor", exitforfunc},
  {"i", ifunc},
  {"j", jfunc},
  {"k", kfunc},
  {"if", iffunc},
  {"else", elsefunc},
  {"endif", endiffunc},
  {"return", returnfunc},
  // output and string ops
  {".", showfunc},
  {"h.", showhexfunc},
  {"..", shownospacefunc},
  {".s", showstackfunc},
  {".rj", showrjfunc},
  {"cr", crfunc},
  {"print", printfunc},
  {"emit", emitfunc},
  {"uemit", uemitfunc},
  {"ord", ordfunc},
  {"tohex", tohexfunc},
  {"bytes32", bytes32func},
  {"free", freefunc},
  {"strlen", strlenfunc},
  {"str=", streqfunc},
  {"str<", strltfunc},
  {"str>", strgtfunc},
  {"strfind", strfindfunc},
  {"strcat", strcatfunc},
  {"strcpy", strcpyfunc},
  {"strtok", strtokfunc},
  {"memset", memsetfunc},
  // time
  {"clock", clockfunc},
  {"sleep", sleepfunc},
  // file
  {"fopen", fileopenfunc},
  {"fread", filereadfunc},
  {"fseek", fileseekfunc},
  {"ftell", filetellfunc},
  {"fwrite", filewritefunc},
  {"fflush", fileflushfunc},
  {"fclose", fileclosefunc},
  {"redirect", redirectfunc},
  {"resetout", resetoutfunc},
  {"flush", flushoutfunc},
  // raw variables in memory
  {"!", pokefunc},
  {"@", peekfunc},
  // named constants & variables
  {"const", constantfunc},
  {"var", variablefunc},
  {"allot", allotfunc},
  {"create", createfunc},
  {",", commafunc},
  {"words", showdefined}
};


void show_primitivesfunc() {
    printf("\n");
    int prim_size = sizeof(primitives) / sizeof(primitives[0]);
    printf("There are currently %i primitives implemented:\n", prim_size - 1);
    for (int x=0; x < (prim_size - 1); x++) {
        printf("%s ", primitives[x].name);
    }
    printf(": ;\n\n");
    printf("Strings are written by simply typing a string literal in double-quotes, e.g. \"Hello there!\".\n\n");
}
