const char *illegal[] = {"times", "again", "exittimes",
                         "for", "next", "exitfor"};
DCLANG_INT num_illegal = sizeof(illegal) / sizeof(illegal[0]);

const char *special[] = {"if", "else", "endif"};
DCLANG_INT num_special = sizeof(special) / sizeof(special[0]);


/* function to validate and return an error message if we are using control
 * structures outside of a definition */
DCLANG_INT validate(const char *token)
{
    DCLANG_INT checkval = 1;
    for (DCLANG_INT i=0; i < num_illegal; i++) {
        if (strcmp(token, illegal[i]) == 0) {
            printf("Error: '%s' -- illegal outside of function def.\n",
                   illegal[i]);
            checkval = 0;
            return checkval;
        }
    }
    return checkval;
}


/* conditionals are 'special forms' that need to be handled in a certain
   way by the compilation process: */
DCLANG_INT is_special_form(const char *token)
{
    DCLANG_INT checkval = 0;
    for (DCLANG_INT i=0; i < num_special; i++) {
        if (strcmp(token, special[i]) == 0) {
            checkval = 1;
            return checkval;
        }
    }
    return checkval;
}


// Function to compile (or interpret) each incoming token
void compile_or_interpret(const char *argument)
{
    char *endPointer = 0;
    double d;
    const struct primitive *pr = primitives;

    if (argument == 0) {
        return;
    }

    // Search user-defined functions (words)
    int found = dclang_findword(argument);
    if (found > -1) {
        if (def_mode) {
            prog[iptr].function.with_param = callword;
            prog[iptr++].param = found;
        } else {
            dclang_callword(found);
        }
        return;
    }

    // Search for a primitive word
    while (pr->name != 0) {
        if (strcmp(pr->name, argument) == 0) {
            if ((def_mode) && (!is_special_form(pr->name))) {
                prog[iptr++].function.without_param = pr->function;
            } else {
                if (validate(argument)) {
                    (*(pr->function)) ();
                }
            }
            return;
        }
        pr++;
    }

    // Neither user word nor primitive word was found.
    // OK, so next, try to convert to a number
    d = strtod(argument, &endPointer);
    if (endPointer != argument) {
        if (def_mode) {
            prog[iptr].function.with_param = push;
            prog[iptr++].param = d;
        } else {
            push(d);
        }
        return;
    }

    // Nothing found, we've reached an error condition.
    // Report the situation and reset the stack.
    data_stack_ptr = 0;
    printf("%s -- syntax error.\n", argument);
    return;
}


void repl() {
    char *token;
    while (strcmp(token = get_token(), "EOF")) {
        // are we dealing with a function definition?
        if (strcmp(token, ":") == 0) {
            startword();
            def_mode = 1;
            continue; // goto top of loop
        }
        if (strcmp(token, ";") == 0) {
            endword();
            def_mode = 0;
            continue; // goto top of loop
        }
        // 'compile' it, or interpret it on-the-fly
        compile_or_interpret(token);
    }
    compile_or_interpret(0);
    // return EXIT_SUCCESS;
}


int dclang_import(char *infilestr) {
    char *prefix = getenv("DCLANG_LIBS");
    if (prefix == NULL) {
        printf("DCLANG_LIBS env variable is unset!\n");
        return -1;
    }
    // check existence of file:
    if (access(infilestr, F_OK) == 0) {
        FILE *infile;
        infile = fopen(infilestr, "r");
        setinput(infile);
        repl();
        return 0;
    }
    char *full_path = malloc(512);
    memset(full_path, 0, 512);
    char *slash = "/";
    strcat(full_path, prefix);
    strcat(full_path, slash);
    strcat(full_path, infilestr);
    if (access(full_path, F_OK) == 0) {
        FILE *infile = fopen(full_path, "r");
        setinput(infile);
        repl();
        return 0;
    }
    printf(
        "The file named %s doesn't appear to exist in the current " \
        "directory, or under %s as %s, or cannot be accessed.\n"
        "You may want to check its existence and permissions!\n",
        infilestr, prefix, full_path
    );
    return -1;
}


int importfunc() {
    if (data_stack_ptr < 1) {
        printf("import -- stack underflow! ");
        return -1;
    }
    char *importfile = (char *)(unsigned long) dclang_pop();
    return dclang_import(importfile);
}

// a small buffer for use by `grabinput`
char string_pad[512];
int string_here;

void grabinput() {
    setinput(stdin);
    char ch;
    // get a starting marker for length
    unsigned long string_start = string_here;
    // bypass leading whitespace
    do {
        if((ch = fgetc(ifp)) == EOF) {
            exit(0);
        }
    } while(isspace(ch));
    while (! strchr("\n", ch)) {
        if (strchr("\\", ch)) {
            // consume an extra char due to backslash
            if ((ch = getchar()) == EOF) exit(0);
            // backspace
            if (strchr("b", ch)) {
                ch = 8;
            }
            if (strchr("n", ch)) {
                ch = 10;
            }
            if (strchr("t", ch)) {
                ch = 9;
            }
        }
        string_pad[string_here++] = ch;
        if ((ch = getchar()) == EOF) exit(0);
    }
    DCLANG_UINT string_addr = (DCLANG_UINT) string_start;
    DCLANG_UINT string_size = (DCLANG_UINT)(string_here - string_start);
    char *string_dest = malloc(string_size + 1);
    char nullstr[] = "\0";
    memcpy(string_dest, (char *)((DCLANG_UINT)&string_pad[0] + string_addr), string_size);
    DCLANG_UINT string_dest_uint = (DCLANG_UINT) string_dest;
    if (string_dest_uint < MIN_STR || MIN_STR == 0) {
        MIN_STR = string_dest_uint;
    }
    if (string_dest_uint + string_size + 1 > MAX_STR || MAX_STR == 0) {
        MAX_STR = string_dest_uint + string_size + 1;
    }
    push(string_dest_uint);
    revertinput();
}

void inputfunc() {
    if (def_mode) {
        prog[iptr++].function.without_param = grabinput;
    } else {
        grabinput();
    }
}
