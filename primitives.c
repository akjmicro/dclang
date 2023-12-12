
void add_primitive(char *name, char *category, void *func_ptr)
{
    primitives_idx += 1;
    if (primitives_idx > 0)
    {
        primitives = realloc(primitives, (primitives_idx + 2) * sizeof(primitive));
    }
    (primitives + primitives_idx)->name = name;
    (primitives + primitives_idx)->function = func_ptr;
    (primitives + primitives_idx)->category = category;
};

void add_all_primitives()
{
    primitives = malloc(sizeof(primitive));
    // boolean logic
    add_primitive("null", "Boolean", nullfunc);
    add_primitive("false", "Boolean", falsefunc);
    add_primitive("true", "Boolean", truefunc);
    add_primitive("=", "Boolean", eqfunc);
    add_primitive("<>", "Boolean", noteqfunc);
    add_primitive("<", "Boolean", ltfunc);
    add_primitive(">", "Boolean", gtfunc);
    add_primitive("<=", "Boolean", ltefunc);
    add_primitive(">=", "Boolean", gtefunc);
    add_primitive("assert", "Boolean", assertfunc);
    // bit manipulation
    add_primitive("and", "Bit manipulation", andfunc);
    add_primitive("or", "Bit manipulation", orfunc);
    add_primitive("xor", "Bit manipulation", xorfunc);
    add_primitive("not", "Bit manipulation", notfunc);
    add_primitive("<<", "Bit manipulation", lshiftfunc);
    add_primitive(">>", "Bit manipulation", rshiftfunc);
    // basic arithmetic
    add_primitive("+", "Arithmetic", addfunc);
    add_primitive("-", "Arithmetic", subfunc);
    add_primitive("*", "Arithmetic", mulfunc);
    add_primitive("/", "Arithmetic", divfunc);
    add_primitive("%", "Arithmetic", modfunc);
    add_primitive("abs", "Arithmetic", absfunc);
    add_primitive("min", "Arithmetic", minfunc);
    add_primitive("max", "Arithmetic", maxfunc);
    // float -> int
    add_primitive("round", "Float -> Integer", roundfunc);
    add_primitive("ceil", "Float -> Integer", ceilfunc);
    add_primitive("floor", "Float -> Integer", floorfunc);
    // higher math
    add_primitive("pow", "Higher Math", powerfunc);
    add_primitive("sqrt", "Higher Math", sqrtfunc);
    add_primitive("exp", "Higher Math", expfunc);
    add_primitive("log", "Higher Math", logfunc);
    add_primitive("log2", "Higher Math", log2func);
    add_primitive("log10", "Higher Math", log10func);
    add_primitive("e", "Higher Math", efunc);
    // trig
    add_primitive("pi", "Trigonometry", pifunc);
    add_primitive("sin", "Trigonometry", sinefunc);
    add_primitive("cos", "Trigonometry", cosfunc);
    add_primitive("tan", "Trigonometry", tanfunc);
    add_primitive("asin", "Trigonometry", asinefunc);
    add_primitive("acos", "Trigonometry", acosfunc);
    add_primitive("atan", "Trigonometry", atanfunc);
    add_primitive("atan2", "Trigonometry", atan2func);
    add_primitive("sinh", "Trigonometry", atanfunc);
    add_primitive("cosh", "Trigonometry", atanfunc);
    add_primitive("tanh", "Trigonometry", atanfunc);
    // randomness
    add_primitive("rand", "Randomness", randfunc);
    // stack manipulation
    add_primitive("drop", "Stack Ops", dropfunc);
    add_primitive("dup", "Stack Ops", dupfunc);
    add_primitive("over", "Stack Ops", overfunc);
    add_primitive("pick", "Stack Ops", pickfunc);
    add_primitive("swap", "Stack Ops", swapfunc);
    add_primitive("2drop", "Stack Ops", drop2func);
    add_primitive("2dup", "Stack Ops", dup2func);
    add_primitive("2over", "Stack Ops", over2func);
    add_primitive("depth", "Stack Ops", depthfunc);
    add_primitive("clear", "Stack Ops", clearfunc);
    add_primitive(".", "Stack Ops", showfunc);
    add_primitive("..", "Stack Ops", shownospacefunc);
    add_primitive(".rj", "Stack Ops", showrjfunc);
    add_primitive(".pz", "Stack Ops", showpzfunc);
    add_primitive(".s", "Stack Ops", showstackfunc);
    // the extra "save" stack
    add_primitive("svpush", "2nd Stack Ops", svpushfunc);
    add_primitive("svpop", "2nd Stack Ops", svpopfunc);
    add_primitive("svdrop", "2nd Stack Ops", svdropfunc);
    add_primitive("svpick", "2nd Stack Ops", svpickfunc);
    add_primitive("svdepth", "2nd Stack Ops", svdepthfunc);
    add_primitive("svclear", "2nd Stack Ops", svclearfunc);
    // variables and arrays
    add_primitive("var", "Variables & Arrays", variablefunc);
    add_primitive("!", "Variables & Arrays", pokefunc);
    add_primitive("@", "Variables & Arrays", peekfunc);
    add_primitive("allot", "Variables & Arrays", allotfunc);
    add_primitive("create", "Variables & Arrays", createfunc);
    add_primitive(",", "Variables & Arrays", commafunc);
    add_primitive("here", "Variables & Arrays", herefunc);
    add_primitive("const", "Variables & Arrays", constantfunc);
    add_primitive("envget", "Variables & Arrays", envgetfunc);
    add_primitive("envset", "Variables & Arrays", envsetfunc);
    // sorting
    add_primitive("sortnums", "Array Sorting", sortnumsfunc);
    add_primitive("sortstrs", "Array Sorting", sortstrsfunc);
    // hash set & get
    add_primitive("h!", "Hashes", hashsetfunc);
    add_primitive("h@", "Hashes", hashgetfunc);
    add_primitive("hkeys", "Hashes", hashkeysfunc);
    // tree storage
    add_primitive("tmake", "Trees", treemakefunc);
    add_primitive("t!", "Trees", treesetfunc);
    add_primitive("t@", "Trees", treegetfunc);
    add_primitive("twalk", "Trees", treewalkfunc);
    add_primitive("tdelete", "Trees", treedelfunc);
#ifdef HAS_TREEDESTROY
    add_primitive("tdestroy", "Trees", treedestroyfunc);
#endif
    // branching/control
    add_primitive("times", "Branching", timesfunc);
    add_primitive("again", "Branching", againfunc);
    add_primitive("exittimes", "Branching", exittimesfunc);
    add_primitive("for", "Branching", forfunc);
    add_primitive("next", "Branching", nextfunc);
    add_primitive("exitfor", "Branching", exitforfunc);
    add_primitive("i", "Branching", ifunc);
    add_primitive("j", "Branching", jfunc);
    add_primitive("k", "Branching", kfunc);
    add_primitive("if", "Branching", iffunc);
    add_primitive("else", "Branching", elsefunc);
    add_primitive("endif", "Branching", endiffunc);
    add_primitive("return", "Branching", returnfunc);
    // output and string ops
    add_primitive("cr", "Strings", crfunc);
    add_primitive("print", "Strings", printfunc);
    add_primitive("emit", "Strings", emitfunc);
    add_primitive("uemit", "Strings", uemitfunc);
    add_primitive("ord", "Strings", ordfunc);
    add_primitive("tohex", "Strings", tohexfunc);
    add_primitive("tonum", "Strings", tonumfunc);
    add_primitive("tostr", "Strings", tostrfunc);
    add_primitive("bytes32", "Strings", bytes32func);
    add_primitive("strlen", "Strings", strlenfunc);
    add_primitive("str=", "Strings", streqfunc);
    add_primitive("str<", "Strings", strltfunc);
    add_primitive("str>", "Strings", strgtfunc);
    add_primitive("strfind", "Strings", strfindfunc);
    add_primitive("strspn", "Strings", strspnfunc);
    add_primitive("strcspn", "Strings", strcspnfunc);
    add_primitive("strtok", "Strings", strtokfunc);
    add_primitive("mkbuf", "Strings", mkbuffunc);
    add_primitive("free", "Strings", freefunc);
    add_primitive("mempcpy", "Strings", mempcpyfunc);
    add_primitive("memset", "Strings", memsetfunc);
    // regex
    add_primitive("regcomp", "Regex", regcompfunc);
    add_primitive("regexec", "Regex", regexecfunc);
    add_primitive("regread", "Regex", regreadfunc);
    // character types
    add_primitive("isalnum", "Character Types", isalnumfunc);
    add_primitive("isalpha", "Character Types", isalphafunc);
    add_primitive("iscntrl", "Character Types", iscntrlfunc);
    add_primitive("isdigit", "Character Types", isdigitfunc);
    add_primitive("isgraph", "Character Types", isgraphfunc);
    add_primitive("islower", "Character Types", islowerfunc);
    add_primitive("isprint", "Character Types", isprintfunc);
    add_primitive("ispunct", "Character Types", ispunctfunc);
    add_primitive("isspace", "Character Types", isspacefunc);
    add_primitive("isupper", "Character Types", isupperfunc);
    add_primitive("isxdigit", "Character Types", isxdigitfunc);
    add_primitive("tolower", "Character Types", tolowerfunc);
    add_primitive("toupper", "Character Types", toupperfunc);
    // file
    add_primitive("fopen", "Files", fileopenfunc);
    add_primitive("fread", "Files", filereadfunc);
    add_primitive("freadline", "Files", filereadlinefunc);
    add_primitive("freadall", "Files", filereadallfunc);
    add_primitive("fseek", "Files", fileseekfunc);
    add_primitive("ftell", "Files", filetellfunc);
    add_primitive("fwrite", "Files", filewritefunc);
    add_primitive("fflush", "Files", fileflushfunc);
    add_primitive("fclose", "Files", fileclosefunc);
    add_primitive("redirect", "Files", redirectfunc);
    add_primitive("resetout", "Files", resetoutfunc);
    add_primitive("flush", "Files", flushoutfunc);
    // low-level (OS) file ops:
    add_primitive("open", "Files", openfunc);
    add_primitive("read", "Files", readfunc);
    add_primitive("write", "Files", writefunc);
    add_primitive("close", "Files", closefunc);
    // tcp networking using sockets
    add_primitive("tcplisten", "Sockets", tcplistenfunc);
    add_primitive("tcpaccept", "Sockets", tcpacceptfunc);
    add_primitive("tcpconnect", "Sockets", tcpconnectfunc);
    // time
    add_primitive("clock", "Time", clockfunc);
    add_primitive("sleep", "Time", sleepfunc);
    add_primitive("epoch", "Time", epochfunc);
    add_primitive("dt->epoch", "Time", dt_to_epochfunc);
    add_primitive("epoch->dt", "Time", epoch_to_dtfunc);
    // block a SIGINT
    add_primitive("block_sigint", "Interrupt Blocking", blocksigintfunc);
    add_primitive("unblock_sigint", "Interrupt Blocking", unblocksigintfunc);
    // portmidi
    add_primitive("_pm_list", "MIDI", _pm_listfunc);
    add_primitive("_pm_open_out", "MIDI", _pm_openoutfunc);
    add_primitive("_pm_ws", "MIDI", _pm_wsfunc);
    add_primitive("_pm_wsr", "MIDI", _pm_wsrfunc);
    add_primitive("_pm_close", "MIDI", _pm_closefunc);
    add_primitive("_pm_terminate", "MIDI", _pm_terminatefunc);
    // os fork and exit
    add_primitive("fork", "Operating System", forkfunc);
    add_primitive("exit", "Operating System", exitfunc);
    // show defined words!
    add_primitive("words", "Other", showdefined);
};

void show_primitivesfunc()
{
    printf("\nThere are currently %i primitives implemented.\n", primitives_idx);
    printf("The following primitives are visible; invisible primitives start "
           "with '_' and are meant to be used privately by libraries:\n"
    );
    const struct primitive *pr = primitives;
    char *old_category = " ";
    char *new_category;
    for (int x=0; x < primitives_idx; x++)
    {
        new_category = (pr + x)->category;
        if (((pr + x)->name)[0] != '_')
        {
            if (strcmp(old_category, new_category))
            {
                printf("\n%-19s| ", new_category);
            }
            printf("%s ", (pr + x)->name);
        }
        old_category = new_category;
    }
    printf(": ;\n\n");
    printf("Strings are written by simply typing a string literal in double-quotes, e.g. \"Hello there!\".\n\n");
}
