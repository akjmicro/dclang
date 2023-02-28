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
void compile_or_interpret(const char *token)
{
    char *endPointer = 0;
    double d;
    const struct primitive *pr = primitives;

    if (token == 0) {
        return;
    }

    // Search user-defined functions (words)
    DCLANG_INT found = dclang_findword(token);
    if (found > -1) {
        if (def_mode) {
            if (strcmp(user_words[num_user_words - 1].name, token) == 0) {
                // If the word found is the word immediately being
                // defined, we can avoid `callword`, because that consumes
                // the return stack. Instead, unconditionally jump
                // to the start of the definition.
                prog[iptr].function.with_param = jumpufunc;
                prog[iptr++].param = found;
            } else {
                prog[iptr].function.with_param = callword;
                prog[iptr++].param = found;
            }
        } else {
            dclang_callword(found);
        }
        return;
    }

    // Search for a primitive word
    while (pr->name != 0) {
        if (strcmp(pr->name, token) == 0) {
            if ((def_mode) && (!is_special_form(pr->name))) {
                prog[iptr++].function.without_param = pr->function;
            } else {
                if (validate(token)) {
                    (*(pr->function)) ();
                }
            }
            return;
        }
        pr++;
    }

    // Neither user word nor primitive word was found.
    // OK, so next, try to convert to a number
    d = strtod(token, &endPointer);
    if (endPointer != token) {
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
    printf("%s -- syntax error.\n", token);
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


DCLANG_INT dclang_import(char *infilestr) {
    char *prefix = getenv("DCLANG_LIBS");
    if (prefix == NULL) {
        printf("DCLANG_LIBS env variable is unset!\n");
        return -1;
    }
    // check existence of file:
    if (access(infilestr, F_OK) == 0) {
        FILE *infile;
        printf("import file found, opening\n");
        infile = fopen(infilestr, "r");
        printf("setting input\n");
        setinput(infile);
        printf("starting repl()\n");
        repl();
        return 0;
    }
    char *full_path = malloc(512);
    printf("doing memset 512\n");
    memset(full_path, 0, 512);
    char *slash = "/";
    printf("starting strcat block\n");
    strcat(full_path, prefix);
    strcat(full_path, slash);
    strcat(full_path, infilestr);
    printf("ending strcat block\n");
    if (access(full_path, F_OK) == 0) {
        printf("full-path import file found, opening\n");
        FILE *infile = fopen(full_path, "r");
        sleep(1);
        printf("full-path setting input\n");
        setinput(infile);
        sleep(1);
        printf("full-path starting repl()\n");
        repl();
        sleep(1);
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
    DCLANG_PTR string_addr = (DCLANG_PTR) string_start;
    DCLANG_PTR string_size = (DCLANG_PTR)(string_here - string_start);
    char *string_dest = malloc(string_size + 1);
    char nullstr[] = "\0";
    memcpy(string_dest, (char *)((DCLANG_PTR)&string_pad[0] + string_addr), string_size);
    DCLANG_PTR string_dest_PTR = (DCLANG_PTR) string_dest;
    if (string_dest_PTR < MIN_STR || MIN_STR == 0) {
        MIN_STR = string_dest_PTR;
    }
    if (string_dest_PTR + string_size + 1 > MAX_STR || MAX_STR == 0) {
        MAX_STR = string_dest_PTR + string_size + 1;
    }
    push(string_dest_PTR);
    revertinput();
}

void inputfunc() {
    if (def_mode) {
        prog[iptr++].function.without_param = grabinput;
    } else {
        grabinput();
    }
}
