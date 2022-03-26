char **hashwords;
DCLANG_UINT hashwords_size = 32;
DCLANG_UINT hashwords_cnt = 0;

static void pokefunc()
{
    if (data_stack_ptr < 2)
    {
        printf("! -- stack underflow! ");
        return;
    }
    DCLANG_UINT idx = (DCLANG_UINT) dclang_pop();
    if (idx < 0 || idx > NUMVARS)
    {
        printf("! -- variable slot number out-of-range!\n");
        return;
    }
    DCLANG_FLT val = dclang_pop();
    vars[idx] = val;
}

static void peekfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("@ -- stack underflow! ");
        return;
    }
    DCLANG_INT idx = (DCLANG_INT) dclang_pop();
    if (idx < 0 || idx > NUMVARS)
    {
        printf("@ -- variable slot number out-of-range!\n");
        return;
    }
    push(vars[idx]);
}

/* implement constants */
static void constantfunc()
{
    startword();
    prog[iptr].function.with_param = push;
    prog[iptr++].param = dclang_pop();
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
    if (iptr < max_iptr)
    {
        // mark current position
        DCLANG_UINT ret_iptr = iptr;
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
    if (data_stack_ptr < 1)
    {
        printf("allot -- stack underflow! ");
        return;
    }
    varsidx += (DCLANG_UINT) dclang_pop() - 1;
}

static void createfunc()
{
    variablefunc();
    --varsidx;
}

static void commafunc()
{
    if (data_stack_ptr < 1)
    {
        printf(", -- stack underflow! ");
        return;
    }
    DCLANG_FLT val = dclang_pop();
    vars[varsidx++] = val;
}

static void herefunc()
{
    push((DCLANG_UINT) varsidx);
}

/* global hash space, a la 'redis', but in local memory */

static void _add_key(char *key){
    if (hashwords_cnt > hashwords_size)
    {
        hashwords_size *= 2;
        hashwords = realloc(hashwords, hashwords_size * sizeof(*hashwords));
    }
    //hashwords[hashwords_cnt] = (char*)malloc(1+strlen(key));
    hashwords[hashwords_cnt] = key;
    ++hashwords_cnt;
}

static void hashsetfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("h! -- stack underflow! ");
        return;
    }
    /* grab the key */
    char *key = (char *)(DCLANG_UINT)dclang_pop();
    DCLANG_UINT key_addr = (DCLANG_UINT) key;
    if (key_addr < MIN_STR || key_addr > MAX_STR)
    {
        perror("h! -- String address for hash key out-of-range.");
        return;
    }
    /* grab the value */
    char *data = (char *)(DCLANG_UINT) dclang_pop();
    ENTRY item = {key, data};
    /* see if we have an entry for the given key first */
    ENTRY *entry = hsearch(item, FIND);
    if (entry == NULL)
    {
        hsearch(item, ENTER);
        _add_key(key);
    } else {
        //free(entry->data);
        entry->data = data;
    }
}

static void hashgetfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("h@ -- stack underflow! ");
        return;
    }
    /* grab the key */
    char *key = (char *)(DCLANG_UINT)dclang_pop();
    DCLANG_UINT key_addr = (DCLANG_UINT) key;
    if (key_addr < MIN_STR || key_addr > MAX_STR)
    {
        perror("h@ -- String address for hash key out-of-range.");
        return;
    }
    /* grab the value */
    char *data = "\0";
    ENTRY item = {key, data};
    /* see if we have an entry for the given key first */
    ENTRY *entry = hsearch(item, FIND);
    if (entry == NULL)
    {
        push(0);
    } else {
        push((DCLANG_UINT)(char *)entry->data);
    }
}

static void hashkeysfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("hkeys -- need an integer index on stack. Stack underflow! ");
        return;
    }
    /* grab the key index */
    DCLANG_UINT keyidx = (DCLANG_UINT)dclang_pop();
    push((DCLANG_UINT)hashwords[keyidx]);
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
    const char *sa = (const char *) (DCLANG_UINT) * (DCLANG_FLT *) a;
    const char *sb = (const char *) (DCLANG_UINT) * (DCLANG_FLT *) b;
    return strcmp(sa, sb);
}

// end helper functions

static void sortnumsfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("sortnums -- need <arrstart_index> <size> on the stack.\n");
        return;
    }
    int size = (DCLANG_UINT) dclang_pop();
    int arrstart = (DCLANG_UINT) dclang_pop();
    qsort (vars+arrstart, size, sizeof(DCLANG_FLT), compare_doubles);
}

static void sortstrsfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("sortstrs -- need <arrstart_index> <size> on the stack.\n");
    }
    int size = (DCLANG_UINT) dclang_pop();
    int arrstart = (DCLANG_UINT) dclang_pop();
    qsort (vars+arrstart, size, sizeof(DCLANG_FLT), compare_strings);
}

// environment variables:

static void envgetfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("envget -- need <env_key> string on the stack.\n");
    }
    /* grab the key */
    char *env_key = (char *)(DCLANG_UINT)dclang_pop();
    DCLANG_UINT env_key_addr = (DCLANG_UINT) env_key;
    if (env_key_addr < MIN_STR || env_key_addr > MAX_STR)
    {
        perror("envget -- String address for hash key out-of-range.");
        return;
    }
    char *val = getenv(env_key);
    DCLANG_UINT val_addr = (DCLANG_UINT) val;
    if (val_addr > MAX_STR || MAX_STR == 0)
    {
        MAX_STR = val_addr;
    }
    if (val_addr < MIN_STR || MIN_STR == 0)
    {
        MIN_STR = val_addr;
    }
    push(val_addr);
}

static void envsetfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("envset -- need <env_val> <env_key> strings on the stack.\n");
    }
    // grab the key from the stack
    char *env_key = (char *)(DCLANG_UINT)dclang_pop();
    DCLANG_UINT env_key_addr = (DCLANG_UINT) env_key;
    if (env_key_addr < MIN_STR || env_key_addr > MAX_STR)
    {
        perror("envset -- String address for environment key out-of-range.");
        return;
    }
    // grab the value from the stack
    char *env_val = (char *)(DCLANG_UINT)dclang_pop();
    DCLANG_UINT env_val_addr = (DCLANG_UINT) env_val;
    if (env_val_addr < MIN_STR || env_val_addr > MAX_STR)
    {
        perror("envset -- String address for environment value out-of-range.");
        return;
    }
    // set the key's value
    setenv(env_key, env_val, 1);
    // no value put on stack -- only side effect
}
