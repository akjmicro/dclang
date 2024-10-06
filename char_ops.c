/* utf-8 char buffer */
char utf8_buf[5];

long utf8_encode(char *out, uint64_t utf)
{
    if (utf <= 0x7F)
    {
        // Plain ASCII
        out[0] = (char) utf;
        out[1] = 0;
        return 1;
    }
    else if (utf <= 0x07FF)
    {
        // 2-byte unicode
        out[0] = (char) (((utf >> 6) & 0x1F) | 0xC0);
        out[1] = (char) (((utf >> 0) & 0x3F) | 0x80);
        out[2] = 0;
        return 2;
    }
    else if (utf <= 0xFFFF)
    {
        // 3-byte unicode
        out[0] = (char) (((utf >> 12) & 0x0F) | 0xE0);
        out[1] = (char) (((utf >>  6) & 0x3F) | 0x80);
        out[2] = (char) (((utf >>  0) & 0x3F) | 0x80);
        out[3] = 0;
        return 3;
    }
    else if (utf <= 0x10FFFF)
    {
        // 4-byte unicode
        out[0] = (char) (((utf >> 18) & 0x07) | 0xF0);
        out[1] = (char) (((utf >> 12) & 0x3F) | 0x80);
        out[2] = (char) (((utf >>  6) & 0x3F) | 0x80);
        out[3] = (char) (((utf >>  0) & 0x3F) | 0x80);
        out[4] = 0;
        return 4;
    }
    else {
        // error - use replacement character
        out[0] = (char) 0xEF;
        out[1] = (char) 0xBF;
        out[2] = (char) 0xBD;
        out[3] = 0;
        return 3;
    }
}

int get_unicode_by_hex(char *chbuf, int usize)
{
    char numstr[usize];
    long int status = (long int) fgets(numstr, usize, ifp);
    int ucode = strtol(numstr, NULL, 16);
    int num_bytes_ret = utf8_encode(chbuf, ucode);
    return status;
}

int get_ascii(char *chbuf, int usize)
{
    char numstr[usize];
    long int status = (long int) fgets(numstr, usize, ifp);
    int acode = strtol(numstr, NULL, 16);
    chbuf[0] = (char) acode;
    chbuf[1] = 0;
    return status;
}

//////////////////////////////////////////////////////////////////////////
// Character emitters. No value goes to the stack; output is immediate. //
//////////////////////////////////////////////////////////////////////////

void emitfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("emit -- stack underflow! ");
        return;
    }
    char char_code = (char) dclang_pop();
    fprintf(ofp, "%c", char_code);
    fflush(ofp);
}

void uemitfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("uemit -- stack underflow! ");
        return;
    }
    long unsigned long char_code = (long unsigned long) dclang_pop();
    long ulen = utf8_encode(utf8_buf, char_code);
    fprintf(ofp, "%s", utf8_buf);
    fflush(ofp);
}

void bytes32func()
{
    DCLANG_INT val = (DCLANG_INT) dclang_pop();
    char low = (char) val & 0xff;
    val >>= 8;
    char lowmid = (char) val & 0xff;
    val >>= 8;
    char highmid = (char) val & 0xff;
    val >>= 8;
    char high = (char) val & 0xff;
    fputc(low, ofp);
    fputc(lowmid, ofp);
    fputc(highmid, ofp);
    fputc(high, ofp);
}

///////////////////////
// character testers //
///////////////////////

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
        push((DCLANG_LONG) -1);
    } else {
        push((DCLANG_LONG) 0);
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
        push((DCLANG_LONG) -1);
    } else {
        push((DCLANG_LONG) 0);
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
        push((DCLANG_LONG) -1);
    } else {
        push((DCLANG_LONG) 0);
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
        push((DCLANG_LONG) -1);
    } else {
        push((DCLANG_LONG) 0);
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
        push((DCLANG_LONG) -1);
    } else {
        push((DCLANG_LONG) 0);
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
        push((DCLANG_LONG) -1);
    } else {
        push((DCLANG_LONG) 0);
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
        push((DCLANG_LONG) -1);
    } else {
        push((DCLANG_LONG) 0);
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
        push((DCLANG_LONG) -1);
    } else {
        push((DCLANG_LONG) 0);
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
        push((DCLANG_LONG) -1);
    } else {
        push((DCLANG_LONG) 0);
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
        push((DCLANG_LONG) -1);
    } else {
        push((DCLANG_LONG) 0);
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
        push((DCLANG_LONG) -1);
    } else {
        push((DCLANG_LONG) 0);
    }
}
