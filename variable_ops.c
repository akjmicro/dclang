MYFLT myvars[1048576];
MYUINT varsidx;

static void pokefunc()
{
    if (data_stack_ptr < 2) {
        printf("! -- stack underflow! ");
        return;
    }
    MYINT idx = (MYINT) pop();
    MYFLT val = pop();
    myvars[idx] = val;
}

static void peekfunc()
{
    if (data_stack_ptr < 1) {
        printf("@ -- stack underflow! ");
        return;
    }
    MYINT idx = (MYINT) pop();
    push(myvars[idx]);
}

/* implement constants */
static void constantfunc()
{
    startword();
    prog[iptr].function.with_param = push;
    prog[iptr++].param = pop();
    endword();
}

static void _variable_common()
{
    // In reality, a variable is a named word that simply pushes
    // an address to its associated memory
    prog[iptr].function.with_param = push;
    prog[iptr++].param = varsidx++;
}

static void variablefunc()
{
    if (iptr < max_iptr) {
        // mark current position
        MYUINT ret_iptr = iptr;
        // jump to end (when this is called in the future)
        iptr = return_stack[--return_stack_ptr];
        return_stack[return_stack_ptr++] = ret_iptr;
        startword();
        _variable_common();
        // part of endword here:
        prog[iptr++].function.without_param = returnfunc;
        max_iptr = iptr;
        // restore function position
        iptr = return_stack[--return_stack_ptr];
        // restore return stack to new highest position
        return_stack[return_stack_ptr++] = max_iptr;
    } else {
        startword();
        _variable_common();
        endword();
    }
}

static void allotfunc()
{
    if (data_stack_ptr < 1) {
        printf("allot -- stack underflow! ");
        return;
    }
    varsidx += (MYUINT) pop() - 1;
}

static void createfunc()
{
    variablefunc();
    --varsidx;
}

static void commafunc()
{
    if (data_stack_ptr < 1) {
        printf(", -- stack underflow! ");
        return;
    }
    MYFLT val = pop();
    myvars[varsidx++] = val;
}

static void herefunc()
{
    push((MYUINT) varsidx);
}

/* global hash space, a la 'redis', but in local memory */

static void hashsetfunc()
{
    if (data_stack_ptr < 2) {
        printf("h! -- stack underflow! ");
        return;
    }
    /* grab the key */
    char *key = (char *)(MYUINT)pop();
    MYUINT key_addr = (MYUINT) key;
    if (key_addr < MIN_STR || key_addr > MAX_STR) {
        perror("h! -- String address for hash key out-of-range.");
        return;
    }
    /* grab the value */
    char *data = (char *)(MYUINT) pop();
    ENTRY item = {key, data};
    /* see if we have an entry for the given key first */
    ENTRY *entry = hsearch(item, FIND);
    if (entry == NULL) {
        hsearch(item, ENTER);
    } else {
        //free(entry->data);
        entry->data = data;
    }
}


static void hashgetfunc()
{
    if (data_stack_ptr < 1) {
        printf("h@ -- stack underflow! ");
        return;
    }
    /* grab the key */
    char *key = (char *)(MYUINT)pop();
    MYUINT key_addr = (MYUINT) key;
    if (key_addr < MIN_STR || key_addr > MAX_STR) {
        perror("h@ -- String address for hash key out-of-range.");
        return;
    }
    /* grab the value */
    char *data = "\0";
    ENTRY item = {key, data};
    /* see if we have an entry for the given key first */
    ENTRY *entry = hsearch(item, FIND);
    if (entry == NULL) {
        push(0);
    } else {
        push((MYUINT)(char *)entry->data);
    }
}

// helper functions for sorting

int compare_doubles (const void *a, const void *b)
{
    const double *da = (const double *) a;
    const double *db = (const double *) b;
    return (*da > *db) - (*da < *db);
}

int compare_strings (const void *a, const void *b)
{
    const char *sa = (const char *) (MYUINT) * (MYFLT *) a;
    const char *sb = (const char *) (MYUINT) * (MYFLT *) b;
    return strcmp(sa, sb);
}

// end helper functions

static void sortnumsfunc()
{
    if (data_stack_ptr < 2) {
        printf("sortnums -- need <arrstart_index> <size> on the stack.\n");
        return;
    }
    int size = (MYUINT) pop();
    int arrstart = (MYUINT) pop();
    qsort (myvars+arrstart, size, sizeof(MYFLT), compare_doubles);
}


static void sortstrsfunc()
{
    if (data_stack_ptr < 2) {
        printf("sortstrs -- need <arrstart_index> <size> on the stack.\n");
    }
    int size = (MYUINT) pop();
    int arrstart = (MYUINT) pop();
    qsort (myvars+arrstart, size, sizeof(MYFLT), compare_strings);
}
