#include "noheap/ht.c"

ht *global_hash_table;

////////////
// Locals //
////////////

// Function to swap two cells
void swap(DCLANG_PTR *a, DCLANG_PTR *b) {
    DCLANG_PTR temp = *a;
    *a = *b;
    *b = temp;
}

// function to reverse an array
void reverse_array(DCLANG_PTR arr[], int n) {
    // Initialize left to the beginning and right to the end
    int left = 0, right = n;
    // Iterate till left is less than right
    while (left < right) {
        // Swap the elements at left and right position
        swap(&arr[left], &arr[right]);
        // Increment the left pointer
        left++;
        // Decrement the right pointer
        right--;
    }
}

void _set_locals_var(DCLANG_FLT flt_idx)
{
    int idx = (int) flt_idx;
    locals_vals[locals_base_idx + idx] = dclang_pop();
}

void _processlocals()
{
    char *token;
    int locals_idx;
    while (strcmp(token = get_token(), "}")) {
        // Add key to ongoing array of keys
        //printf("Token is: %s\n", token);
        locals_keys[locals_base_idx + locals_idx] = token;
        locals_idx += 1;
    }
    reverse_array(locals_keys + locals_base_idx, locals_idx - 1);
    // For each key up to the count, pop the stack
    // onto the locals_vals array:
    for (int i=0; i<locals_idx; i++) {
        prog[iptr].function.with_param = _set_locals_var;
        prog[iptr++].param = (DCLANG_FLT) i;
    }
}

void _get_locals_var(DCLANG_FLT flt_idx)
{
    int idx = (int) flt_idx;
    push(locals_vals[locals_base_idx + idx]);
}

/////////////
// Globals //
/////////////

void pokefunc()
{
    if (data_stack_ptr < 2)
    {
        printf("! -- stack underflow! ");
        return;
    }
    DCLANG_PTR idx = (DCLANG_PTR) dclang_pop();
    if (idx < 0 || idx > NUMVARS)
    {
        printf("! -- variable slot number out-of-range!\n");
        return;
    }
    DCLANG_FLT val = dclang_pop();
    vars[idx] = val;
}

void peekfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("@ -- stack underflow! ");
        return;
    }
    DCLANG_PTR idx = (DCLANG_PTR) dclang_pop();
    if (idx < 0 || idx > NUMVARS)
    {
        printf("@ -- variable slot number %lu is out-of-range!\n", idx);
        return;
    }
    push(vars[idx]);
}

/* implement constants */
void constantfunc()
{
    const_keys[const_idx] = get_token();
    const_vals[const_idx++] = dclang_pop();
}

void variablefunc()
{
    DCLANG_PTR next_var = vars_idx++;
    var_keys[var_map_idx] = get_token();
    var_vals[var_map_idx++] = next_var;
}

void allotfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("allot -- stack underflow! ");
        return;
    }
    vars_idx += (DCLANG_PTR) dclang_pop() - 1;
}

void createfunc()
{
    variablefunc();
    --vars_idx;
}

void commafunc()
{
    if (data_stack_ptr < 1)
    {
        printf(", -- stack underflow! ");
        return;
    }
    DCLANG_FLT val = dclang_pop();
    vars[vars_idx++] = val;
}

void herefunc()
{
    push((DCLANG_PTR) vars_idx);
}

// some helpers to show stuff

void showvars()
{
    for (int x=0; x < var_map_idx; x++) {
        printf("Variable %i: %s @ %li\n", x, var_keys[x], var_vals[x]);
    }
}

void showconsts()
{
    for (int x=0; x < const_idx; x++) {
        printf("Constant %i: %s ==> %.19g\n", x, const_keys[x], const_vals[x]);
    }
}

//////////////////////////////////////////////////////////
// global hash space, a la 'redis', but in local memory //
//////////////////////////////////////////////////////////

void _add_key(char *key){
    if (hashwords_cnt > hashwords_size)
    {
        hashwords_size *= 2;
        hashwords = dclang_realloc(hashwords, hashwords_size * sizeof(*hashwords));
    }
    hashwords[hashwords_cnt] = key;
    ++hashwords_cnt;
}

void hashsetfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("h! -- stack underflow! ");
        return;
    }
    /* grab the key */
    char *key = (const char *)(DCLANG_PTR) dclang_pop();
    DCLANG_PTR key_addr = (DCLANG_PTR) key;
    if (key_addr < MIN_STR || key_addr > MAX_STR)
    {
        perror("h! -- String address for hash key out-of-range.");
        return;
    }
    /* grab the value */
    void *value = (void *)(DCLANG_PTR) dclang_pop();
    void *confirm = hset(global_hash_table, key, value);
    if (confirm != NULL)
    {
        _add_key(key);
    }
}

void hashgetfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("h@ -- stack underflow! ");
        return;
    }
    /* grab the key */
    char *key = (char *)(DCLANG_PTR) dclang_pop();
    DCLANG_PTR key_addr = (DCLANG_PTR) key;
    if (key_addr < MIN_STR || key_addr > MAX_STR)
    {
        perror("h@ -- String address for hash key out-of-range.");
        return;
    }
    /* grab the value */
    void *value = hget(global_hash_table, key);
    if (value == NULL)
    {
        push(0);
    } else {
        push((DCLANG_PTR)(char *)value);
    }
}

void hashkeysfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("hkeys -- need an integer index on stack. Stack underflow! ");
        return;
    }
    /* grab the key index */
    DCLANG_PTR keyidx = (DCLANG_PTR) dclang_pop();
    push((DCLANG_PTR) hashwords[keyidx]);
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
    const char *sa = (const char *) (DCLANG_PTR) * (DCLANG_FLT *) a;
    const char *sb = (const char *) (DCLANG_PTR) * (DCLANG_FLT *) b;
    return strcmp(sa, sb);
}

// end helper functions

void sortnumsfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("sortnums -- need <arrstart_index> <size> on the stack.\n");
        return;
    }
    int size = (DCLANG_PTR) dclang_pop();
    int arrstart = (DCLANG_PTR) dclang_pop();
    qsort (vars+arrstart, size, sizeof(DCLANG_FLT), compare_doubles);
}

void sortstrsfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("sortstrs -- need <arrstart_index> <size> on the stack.\n");
    }
    int size = (DCLANG_PTR) dclang_pop();
    int arrstart = (DCLANG_PTR) dclang_pop();
    qsort (vars+arrstart, size, sizeof(DCLANG_FLT), compare_strings);
}

// environment variables:

void envgetfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("envget -- need <env_key> string on the stack.\n");
    }
    /* grab the key */
    char *env_key = (char *)(DCLANG_PTR) dclang_pop();
    DCLANG_PTR env_key_addr = (DCLANG_PTR) env_key;
    if (env_key_addr < MIN_STR || env_key_addr > MAX_STR)
    {
        perror("envget -- String address for hash key out-of-range.");
        return;
    }
    char *val = getenv(env_key);
    DCLANG_PTR val_addr = (DCLANG_PTR) val;
    if (val_addr > MAX_STR || MAX_STR == 0)
    {
        MAX_STR = val_addr;
    }
    if ((val_addr != 0) && (val_addr < MIN_STR || MIN_STR == 0))
    {
        MIN_STR = val_addr;
    }
    push(val_addr);
}

void envsetfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("envset -- need <env_val> <env_key> strings on the stack.\n");
    }
    // grab the key from the stack
    char *env_key = (char *)(DCLANG_PTR) dclang_pop();
    DCLANG_PTR env_key_addr = (DCLANG_PTR) env_key;
    if (env_key_addr < MIN_STR || env_key_addr > MAX_STR)
    {
        perror("envset -- String address for environment key out-of-range.");
        return;
    }
    // grab the value from the stack
    char *env_val = (char *)(DCLANG_PTR) dclang_pop();
    DCLANG_PTR env_val_addr = (DCLANG_PTR) env_val;
    if (env_val_addr < MIN_STR || env_val_addr > MAX_STR)
    {
        perror("envset -- String address for environment value out-of-range.");
        return;
    }
    // set the key's value
    setenv(env_key, env_val, 1);
    // no value put on stack -- only side effect
}
