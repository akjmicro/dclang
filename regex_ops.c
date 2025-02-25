// Function to compile a regex pattern
void regcompfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("regcomp -- stack underflow: need <pattern> and <flags> on the stack!\n");
        return;
    }

    // Pop the regex pattern from the stack
    DCLANG_ULONG flags = (DCLANG_ULONG)dclang_pop();
    DCLANG_PTR pattern_PTR_addr = (DCLANG_PTR)dclang_pop();

    if (pattern_PTR_addr < MIN_STR || pattern_PTR_addr > MAX_STR)
    {
        perror("regcomp -- Pattern address out-of-range.\n");
        return;
    }

    char *pattern = (char *)pattern_PTR_addr;

    // Compile the regex pattern
    regex_t *regex = (regex_t *)dclang_malloc(sizeof(regex_t));
    if (regcomp(regex, pattern, (int)flags) != 0)
    {
        perror("regcomp -- Error compiling regex pattern\n");
        dclang_free(regex);
        return;
    }

    // Push the compiled regex object pointer onto the stack
    push((DCLANG_PTR)regex);
}

// Function to execute the regex matching
void regexecfunc()
{
    if (data_stack_ptr < 3)
    {
        printf("regexec -- stack underflow; need <regexobj> <string_to_search> <flags> on the stack!\n");
        return;
    }

    // Pop the input string and the compiled regex object from the stack
    DCLANG_LONG flags = (DCLANG_LONG)dclang_pop();
    DCLANG_PTR input_str_PTR_addr = (DCLANG_PTR)dclang_pop();
    DCLANG_PTR regex_obj_PTR = (DCLANG_PTR)dclang_pop();

    if (regex_obj_PTR < 0 || input_str_PTR_addr < MIN_STR || input_str_PTR_addr > MAX_STR)
    {
        perror("regexec -- Invalid regex object or string address.\n");
        return;
    }

    regex_t *regex = (regex_t *)regex_obj_PTR;
    char *input_str = (char *)input_str_PTR_addr;

    // Execute the regex matching
    regmatch_t *match = (regmatch_t *)dclang_malloc(10 * sizeof(regmatch_t));
    if (regexec(regex, input_str, 10, match, (int)flags) == 0)
    {
        // If a match is found, push the match object onto the stack
        push((DCLANG_PTR)match);
    }
    else
    {
        // No match found, push -1 to indicate failure
        push((DCLANG_LONG)-1);
    }
}

void regreadfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("regread -- stack underflow; need <regexec_result> <match_index> on the stack! \n");
        printf("regread will return a start and end index relative to the original searched string. \n");
        printf("If the user actually wants that substring, it makes sense to have saved the original \n");
        printf("string, and put the results of regread at the top of stack, then call `strslice`,\n");
        printf("which needs to be imported from the 'string' module.\n");
        return;
    }

    DCLANG_LONG index = (DCLANG_LONG)dclang_pop();
    DCLANG_PTR regmatch_pnt = (DCLANG_PTR)dclang_pop();

    if ((DCLANG_LONG)regmatch_pnt > 0) {
        regmatch_t *match = (regmatch_t *)regmatch_pnt;
        push((DCLANG_LONG)(match[index].rm_so));
        push((DCLANG_LONG)(match[index].rm_eo));
    } else {
        push((DCLANG_LONG)-1);
        push((DCLANG_LONG)-1);
    }
}
