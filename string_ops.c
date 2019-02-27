char string_pad[1048576];
unsigned long string_here;

static void stringfunc()
{
    long ch;
    /* get a starting marker for length */
    unsigned long string_start = string_here;
    
    if ((ch = fgetc(ifp)) == EOF) exit(0);
    while (! strchr("\"", ch)) {
        if (strchr("\\", ch)) {
            /* consume an extra char due to backslash */
            if ((ch = fgetc(ifp)) == EOF) exit(0);
            /* backspace */
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
        if ((ch = fgetc(ifp)) == EOF) exit(0);
    }
    double string_addr = \
        (double)((unsigned long)&string_pad + string_start);
    double string_size = (double)(string_here - string_start);
    if (def_mode) {
        prog[iptr].function.with_param = push;
        prog[iptr++].param = string_addr;
        prog[iptr].function.with_param = push;
        prog[iptr++].param = string_size; 
    } else {
        push(string_addr);
        push(string_size);
    }
}


static void printfunc()
{
    if (data_stack_ptr < 2) {
        printf("stack underflow! ");
        return;
    }
    unsigned long str_len = (unsigned long) pop();
    char *str_start = (char *)((unsigned long) pop());
    char dest[str_len + 1];
    char nullstr[] = "\0";
    memcpy(dest, (char *)(str_start + 1), str_len);
    memcpy(dest + str_len, (char *)nullstr, 1);
    printf("%s", dest);
    fflush(stdout);
}


static void emitfunc()
{
    long char_code = (long) pop();
    char charbuf[2] = { char_code, 0 };
    printf("%s", charbuf);
    fflush(stdout);
}

/* utf-8 char buffer */
char utf8_buf[5];


static long utf8_encode(char *out, uint64_t utf)
{
    if (utf <= 0x7F) {
        // Plain ASCII
        out[0] = (char) utf;
        out[1] = 0;
        return 1;
    }
    else if (utf <= 0x07FF) {
        // 2-byte unicode
        out[0] = (char) (((utf >> 6) & 0x1F) | 0xC0);
        out[1] = (char) (((utf >> 0) & 0x3F) | 0x80);
        out[2] = 0;
        return 2;
    }
    else if (utf <= 0xFFFF) {
        // 3-byte unicode
        out[0] = (char) (((utf >> 12) & 0x0F) | 0xE0);
        out[1] = (char) (((utf >>    6) & 0x3F) | 0x80);
        out[2] = (char) (((utf >>    0) & 0x3F) | 0x80);
        out[3] = 0;
        return 3;
    }
    else if (utf <= 0x10FFFF) {
        // 4-byte unicode
        out[0] = (char) (((utf >> 18) & 0x07) | 0xF0);
        out[1] = (char) (((utf >> 12) & 0x3F) | 0x80);
        out[2] = (char) (((utf >>    6) & 0x3F) | 0x80);
        out[3] = (char) (((utf >>    0) & 0x3F) | 0x80);
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


static void uemitfunc()
{
    long unsigned long char_code = (long unsigned long) pop();
    long ulen = utf8_encode(utf8_buf, char_code);
    printf("%s", utf8_buf);
    fflush(stdout);
}
