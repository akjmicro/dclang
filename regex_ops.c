// Function to compile a regex pattern
void regcompfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("regcomp -- stack underflow: need <pattern> and <flags> on the stack! ");
        return;
    }

    // Pop the regex pattern from the stack
    DCLANG_UINT flags = (DCLANG_UINT)dclang_pop();
    DCLANG_PTR pattern_PTR_addr = (DCLANG_PTR)dclang_pop();

    if (pattern_PTR_addr < MIN_STR || pattern_PTR_addr > MAX_STR)
    {
        perror("regcomp -- Pattern address out-of-range.");
        return;
    }

    char *pattern = (char *)pattern_PTR_addr;

    // Compile the regex pattern
    regex_t *regex = (regex_t *)malloc(sizeof(regex_t));
    if (regcomp(regex, pattern, (int)flags) != 0)
    {
        perror("regcomp -- Error compiling regex pattern");
        free(regex);
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
        printf("regexec -- stack underflow; need <regexobj> <string_to_search> <flags> on the stack! ");
        return;
    }

    // Pop the input string and the compiled regex object from the stack
    DCLANG_INT flags = (DCLANG_INT)dclang_pop();
    DCLANG_PTR input_str_PTR_addr = (DCLANG_PTR)dclang_pop();
    DCLANG_PTR regex_obj_PTR = (DCLANG_PTR)dclang_pop();

    if (regex_obj_PTR < 0 || input_str_PTR_addr < MIN_STR || input_str_PTR_addr > MAX_STR)
    {
        perror("regexec -- Invalid regex object or string address.");
        return;
    }

    regex_t *regex = (regex_t *)regex_obj_PTR;
    char *input_str = (char *)input_str_PTR_addr;

    // Execute the regex matching
    regmatch_t *match = (regmatch_t *)malloc(10 * sizeof(regmatch_t));
    if (regexec(regex, input_str, 10, match, (int)flags) == 0)
    {
        // If a match is found, push the match object onto the stack
        push((DCLANG_PTR)match);
    }
    else
    {
        // No match found, push -1 to indicate failure
        push((DCLANG_INT)-1);
    }
}

void regreadfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("regread -- stack underflow; need <regex_exec_result> <index> on the stack! ");
        return;
    }

    DCLANG_INT index = (DCLANG_INT)dclang_pop();
    DCLANG_PTR regmatch_pnt = (DCLANG_PTR)dclang_pop();

    if ((DCLANG_INT)regmatch_pnt > 0) {
        regmatch_t *match = (regmatch_t *)regmatch_pnt;
        push((DCLANG_INT)(match[index].rm_so));
        push((DCLANG_INT)(match[index].rm_eo));
    } else {
        push((DCLANG_INT)-1);
        push((DCLANG_INT)-1);
    }
}
