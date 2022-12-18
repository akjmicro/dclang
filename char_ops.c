
void isalnumfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("isalnum -- stack underflow! ");
        return;
    }
    DCLANG_PTR char_ptr = (DCLANG_PTR) dclang_pop();
    if (isalnum(char_ptr)) {
        push((DCLANG_INT) -1);
    } else {
        push((DCLANG_INT) 0);
    }
}

void isalphafunc()
{
    if (data_stack_ptr < 1)
    {
        printf("isalpha -- stack underflow! ");
        return;
    }
    DCLANG_PTR char_ptr = (DCLANG_PTR) dclang_pop();
    if (isalpha(char_ptr)) {
        push((DCLANG_INT) -1);
    } else {
        push((DCLANG_INT) 0);
    }
}

void iscntrlfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("iscntrl -- stack underflow! ");
        return;
    }
    DCLANG_PTR char_ptr = (DCLANG_PTR) dclang_pop();
    if (iscntrl(char_ptr)) {
        push((DCLANG_INT) -1);
    } else {
        push((DCLANG_INT) 0);
    }
}

void isdigitfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("isdigit -- stack underflow! ");
        return;
    }
    DCLANG_PTR char_ptr = (DCLANG_PTR) dclang_pop();
    if (isdigit(char_ptr)) {
        push((DCLANG_INT) -1);
    } else {
        push((DCLANG_INT) 0);
    }
}

void isgraphfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("isgraph -- stack underflow! ");
        return;
    }
    DCLANG_PTR char_ptr = (DCLANG_PTR) dclang_pop();
    if (isgraph(char_ptr)) {
        push((DCLANG_INT) -1);
    } else {
        push((DCLANG_INT) 0);
    }
}

void islowerfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("islower -- stack underflow! ");
        return;
    }
    DCLANG_PTR char_ptr = (DCLANG_PTR) dclang_pop();
    if (islower(char_ptr)) {
        push((DCLANG_INT) -1);
    } else {
        push((DCLANG_INT) 0);
    }
}

void isprintfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("isprint -- stack underflow! ");
        return;
    }
    DCLANG_PTR char_ptr = (DCLANG_PTR) dclang_pop();
    if (isprint(char_ptr)) {
        push((DCLANG_INT) -1);
    } else {
        push((DCLANG_INT) 0);
    }
}

void ispunctfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("ispunct -- stack underflow! ");
        return;
    }
    DCLANG_PTR char_ptr = (DCLANG_PTR) dclang_pop();
    if (ispunct(char_ptr)) {
        push((DCLANG_INT) -1);
    } else {
        push((DCLANG_INT) 0);
    }
}

void isspacefunc()
{
    if (data_stack_ptr < 1)
    {
        printf("isspace -- stack underflow! ");
        return;
    }
    DCLANG_PTR char_ptr = (DCLANG_PTR) dclang_pop();
    if (isspace(char_ptr)) {
        push((DCLANG_INT) -1);
    } else {
        push((DCLANG_INT) 0);
    }
}

void isupperfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("isupper -- stack underflow! ");
        return;
    }
    DCLANG_PTR char_ptr = (DCLANG_PTR) dclang_pop();
    if (isupper(char_ptr)) {
        push((DCLANG_INT) -1);
    } else {
        push((DCLANG_INT) 0);
    }
}

void isxdigitfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("isxdigit -- stack underflow! ");
        return;
    }
    DCLANG_PTR char_ptr = (DCLANG_PTR) dclang_pop();
    if (isxdigit(char_ptr)) {
        push((DCLANG_INT) -1);
    } else {
        push((DCLANG_INT) 0);
    }
}
