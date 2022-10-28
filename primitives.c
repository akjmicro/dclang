/* we need to populate the dictionary first with primitives */
struct primitive {
  const char *name;
  void (*function) (void);
} primitive;


struct primitive *primitives;
int primitives_idx = -1;


void add_primitive(char *name, void *func_ptr)
{
    primitives_idx += 1;
    if (primitives_idx > 0)
    {
        primitives = realloc(primitives, (primitives_idx + 2) * sizeof(primitive));
    }
    (primitives + primitives_idx) -> name = name;
    (primitives + primitives_idx) -> function = func_ptr;
};


void add_all_primitives()
{
    primitives = malloc(sizeof(primitive));
    // constants
    add_primitive("null", nullfunc);
    add_primitive("false", falsefunc);
    add_primitive("true", truefunc);
    add_primitive("pi", pifunc);
    add_primitive("e", efunc);
    // basic arithmetic
    add_primitive("+", addfunc);
    add_primitive("-", subfunc);
    add_primitive("*", mulfunc);
    add_primitive("/", divfunc);
    add_primitive("%", modfunc);
    add_primitive("abs", absfunc);
    add_primitive("min", minfunc);
    add_primitive("max", maxfunc);
    add_primitive("<<", lshiftfunc);
    add_primitive(">>", rshiftfunc);
    // comparisons
    add_primitive("=", eqfunc);
    add_primitive("<>", noteqfunc);
    add_primitive("<", ltfunc);
    add_primitive(">", gtfunc);
    add_primitive("<=", ltefunc);
    add_primitive(">=", gtefunc);
    // logic
    add_primitive("assert", assertfunc);
    add_primitive("and", andfunc);
    add_primitive("or", orfunc);
    add_primitive("not", notfunc);
    add_primitive("xor", xorfunc);
    // float -> int
    add_primitive("round", roundfunc);
    add_primitive("ceil", ceilfunc);
    add_primitive("floor", floorfunc);
    // higher math
    add_primitive("pow", powerfunc);
    add_primitive("sqrt", sqrtfunc);
    add_primitive("log", logfunc);
    add_primitive("log2", log2func);
    add_primitive("log10", log10func);
    // trig
    add_primitive("sin", sinefunc);
    add_primitive("cos", cosfunc);
    add_primitive("tan", tanfunc);
    // randomness
    add_primitive("rand", randfunc);
    // stack manipulation
    add_primitive("drop", dropfunc);
    add_primitive("dup", dupfunc);
    add_primitive("over", overfunc);
    add_primitive("pick", pickfunc);
    add_primitive("swap", swapfunc);
    add_primitive("2drop", drop2func);
    add_primitive("2dup", dup2func);
    add_primitive("2over", over2func);
    add_primitive("depth", depthfunc);
    add_primitive("clear", clearfunc);
    add_primitive(".", showfunc);
    add_primitive("..", shownospacefunc);
    add_primitive(".rj", showrjfunc);
    add_primitive(".s", showstackfunc);
    // extra stack
    add_primitive("svpush", svpushfunc);
    add_primitive("svpop", svpopfunc);
    add_primitive("svdrop", svdropfunc);
    add_primitive("svpick", svpickfunc);
    add_primitive("svdepth", svdepthfunc);
    add_primitive("svclear", svclearfunc);
    // constants & variables
    add_primitive("!", pokefunc);
    add_primitive("@", peekfunc);
    add_primitive("const", constantfunc);
    add_primitive("var", variablefunc);
    add_primitive("allot", allotfunc);
    add_primitive("create", createfunc);
    add_primitive(",", commafunc);
    add_primitive("here", herefunc);
    add_primitive("envget", envgetfunc);
    add_primitive("envset", envsetfunc);
    // sorting
    add_primitive("sortnums", sortnumsfunc);
    add_primitive("sortstrs", sortstrsfunc);
    // hash set & get
    add_primitive("h!", hashsetfunc);
    add_primitive("h@", hashgetfunc);
    add_primitive("hkeys", hashkeysfunc);
    // tree storage
    add_primitive("treemake", treemakefunc);
    add_primitive("tree!", treesetfunc);
    add_primitive("tree@", treegetfunc);
    add_primitive("treewalk", treewalkfunc);
    add_primitive("treedel", treedelfunc);
    add_primitive("treedestroy", treedestroyfunc);
    // branching
    add_primitive("times", timesfunc);
    add_primitive("again", againfunc);
    add_primitive("exittimes", exittimesfunc);
    add_primitive("for", forfunc);
    add_primitive("next", nextfunc);
    add_primitive("exitfor", exitforfunc);
    add_primitive("i", ifunc);
    add_primitive("j", jfunc);
    add_primitive("k", kfunc);
    add_primitive("if", iffunc);
    add_primitive("else", elsefunc);
    add_primitive("endif", endiffunc);
    add_primitive("return", returnfunc);
    // output and string ops
    add_primitive("cr", crfunc);
    add_primitive("print", printfunc);
    add_primitive("emit", emitfunc);
    add_primitive("uemit", uemitfunc);
    add_primitive("ord", ordfunc);
    add_primitive("tohex", tohexfunc);
    add_primitive("tonum", tonumfunc);
    add_primitive("tostr", tostrfunc);
    add_primitive("bytes32", bytes32func);
    add_primitive("strlen", strlenfunc);
    add_primitive("str=", streqfunc);
    add_primitive("str<", strltfunc);
    add_primitive("str>", strgtfunc);
    add_primitive("strfind", strfindfunc);
    add_primitive("strtok", strtokfunc);
    add_primitive("mkbuf", mkbuffunc);
    add_primitive("free", freefunc);
    add_primitive("mempcpy", mempcpyfunc);
    add_primitive("memset", memsetfunc);
    add_primitive("isalnum", isalnumfunc);
    add_primitive("isalpha", isalphafunc);
    add_primitive("iscntrl", iscntrlfunc);
    add_primitive("isdigit", isdigitfunc);
    add_primitive("isgraph", isgraphfunc);
    add_primitive("islower", islowerfunc);
    add_primitive("isprint", isprintfunc);
    add_primitive("ispunct", ispunctfunc);
    add_primitive("isspace", isspacefunc);
    add_primitive("isupper", isupperfunc);
    add_primitive("isxdigit", isxdigitfunc);
    add_primitive("tolower", tolowerfunc);
    add_primitive("toupper", toupperfunc);
    // file
    add_primitive("fopen", fileopenfunc);
    add_primitive("fread", filereadfunc);
    add_primitive("freadline", filereadlinefunc);
    add_primitive("freadall", filereadallfunc);
    add_primitive("fseek", fileseekfunc);
    add_primitive("ftell", filetellfunc);
    add_primitive("fwrite", filewritefunc);
    add_primitive("fflush", fileflushfunc);
    add_primitive("fclose", fileclosefunc);
    add_primitive("redirect", redirectfunc);
    add_primitive("resetout", resetoutfunc);
    add_primitive("flush", flushoutfunc);
    // low-level (OS) file ops:
    add_primitive("open", openfunc);
    add_primitive("read", readfunc);
    add_primitive("write", writefunc);
    add_primitive("close", closefunc);
    // tcp networking using sockets
    add_primitive("tcplisten", tcplistenfunc);
    add_primitive("tcpaccept", tcpacceptfunc);
    add_primitive("tcpconnect", tcpconnectfunc);
    // time
    add_primitive("clock", clockfunc);
    add_primitive("sleep", sleepfunc);
    add_primitive("epoch", epochfunc);
    add_primitive("dt->epoch", dt_to_epochfunc);
    add_primitive("epoch->dt", epoch_to_dtfunc);
    // block a SIGINT
    add_primitive("block_sigint", blocksigintfunc);
    add_primitive("unblock_sigint", unblocksigintfunc);
    // os fork and exit
    add_primitive("fork", forkfunc);
    add_primitive("exit", exitfunc);
    // show defined words!
    add_primitive("words", showdefined);
};

void show_primitivesfunc()
{
    printf("\nThere are currently %i primitives implemented:\n", primitives_idx);
    const struct primitive *pr = primitives;
    for (int x=0; x < primitives_idx; x++) {
        printf("%s ", (pr + x) -> name);
    }
    printf(": ;\n\n");
    printf("Strings are written by simply typing a string literal in double-quotes, e.g. \"Hello there!\".\n\n");
}
