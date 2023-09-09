
void isalnumfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("isalnum -- stack underflow! ");
        return;
    }
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("isalnum -- String address out-of-range.");
        return;
    }
    char *char_ptr = (char *) string_PTR_addr;
    if (isalnum(char_ptr[0])) {
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
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("isalpha -- String address out-of-range.");
        return;
    }
    char *char_ptr = (char *) string_PTR_addr;
    if (isalpha(char_ptr[0])) {
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
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("iscntrl -- String address out-of-range.");
        return;
    }
    char *char_ptr = (char *) string_PTR_addr;
    if (iscntrl(char_ptr[0])) {
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
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("isdigit -- String address out-of-range.");
        return;
    }
    char *char_ptr = (char *) string_PTR_addr;
    if (isdigit(char_ptr[0])) {
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
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("isgraph -- String address out-of-range.");
        return;
    }
    char *char_ptr = (char *) string_PTR_addr;
    if (isgraph(char_ptr[0])) {
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
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("islower -- String address out-of-range.");
        return;
    }
    char *char_ptr = (char *) string_PTR_addr;
    if (islower(char_ptr[0])) {
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
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("isprint -- String address out-of-range.");
        return;
    }
    char *char_ptr = (char *) string_PTR_addr;
    if (isprint(char_ptr[0])) {
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
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("ispunct -- String address out-of-range.");
        return;
    }
    char *char_ptr = (char *) string_PTR_addr;
    if (ispunct(char_ptr[0])) {
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
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("isspace -- String address out-of-range.");
        return;
    }
    char *char_ptr = (char *) string_PTR_addr;
    if (isspace(char_ptr[0])) {
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
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("isupper -- String address out-of-range.");
        return;
    }
    char *char_ptr = (char *) string_PTR_addr;
    if (isupper(char_ptr[0])) {
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
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("isxdigit -- String address out-of-range.");
        return;
    }
    char *char_ptr = (char *) string_PTR_addr;
    if (isxdigit(char_ptr[0])) {
        push((DCLANG_INT) -1);
    } else {
        push((DCLANG_INT) 0);
    }
}
