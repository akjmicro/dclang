const char *illegal[] = {"times", "again", "exittimes",
                         "for", "next", "exitfor"};
MYINT num_illegal = sizeof(illegal) / sizeof(illegal[0]);

const char *special[] = {"if", "else", "endif"};
MYINT num_special = sizeof(special) / sizeof(special[0]);


/* function to validate and return an error message if we are using control
 * structures outside of a definition */
static MYINT validate(const char *token)
{
    MYINT checkval = 1;
    for (MYINT i=0; i < num_illegal; i++) {
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
static MYINT is_special_form(const char *token)
{
    MYINT checkval = 0;
    for (MYINT i=0; i < num_special; i++) {
        if (strcmp(token, special[i]) == 0) {
            checkval = 1;
            return checkval;
        }
    }
    return checkval;
}

// function to compile (or interpret) each incoming token
static void compile_or_interpret(const char *argument)
{
    char *endPointer = 0;
    double d;
    const struct primitive *pr = primitives;

    if (argument == 0) {
        return;
    }

    // search dictionary (list, not hash) entry
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

    // search user-defined functions (words)
    for (MYINT x = num_user_words - 1; x > -1 ; x--) {
        if (strcmp(user_words[x].name, argument) == 0) {
            if (def_mode) {
                prog[iptr].function.with_param = callfunc;
                prog[iptr++].param = user_words[x].word_start;
            } else {
                MYINT cur_iptr = iptr;
                callfunc(user_words[x].word_start);
                // run the function
                while (iptr < max_iptr) {
                    iptr += 1;
                    (*(prog[iptr].function.with_param)) (prog[iptr].param);
                }
            }
            return;
        }
    }

    /* primitive not found, user definitions not found.  OK, so next, try to
    convert to a number */
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

    /* nothing found, we've reached an error condition, so report
    the situation and reset the stack */
    data_stack_ptr = 0;
    printf("%s -- syntax error.\n", argument);
    return;
}


static void repl() {
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


static void import(char *infilestr) {
    FILE *infile;
    // check existence of file:
    if (access(infilestr, F_OK) == -1) {
        printf("The file named %s doesn't appear to exist, " \
               "or cannot be accessed.\n", infilestr);
        return;
    }
    infile = fopen(infilestr, "r");
    setinput(infile);
    repl();
}


static void importfunc() {
    if (data_stack_ptr < 1) {
        printf("import -- stack underflow! ");
        return;
    }
    char *importfile = (char *)(unsigned long) pop();
    return import(importfile);
}


static void grabinput() {
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
    unsigned long string_addr = (unsigned long) string_start;
    unsigned long string_size = (unsigned long)(string_here - string_start);
    char *string_dest = malloc(string_size + 1);
    // number for stack needs to be a double:
    double string_dest_dbl = (double)(unsigned long) string_dest;
    char nullstr[] = "\0";
    memcpy(string_dest, (char *)((unsigned long)&string_pad[0] + string_addr), string_size);
    push(string_dest_dbl);
}

static void inputfunc() {
    if (!def_mode) {
        grabinput();
    }
}
