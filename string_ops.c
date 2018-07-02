/* unused as of yet */

char string_pad[1048576];
unsigned long int string_here;

static void stringfunc()
{
    int ch;
    /* get a starting marker for length */
    unsigned long int string_start = string_here;
    
    if ((ch = fgetc(stdin)) == EOF) exit(0);
    while (! strchr("\"", ch)) {
        if (strchr("\\", ch)) {
            /* consume an extra char due to backslash */            
            if ((ch = fgetc(stdin)) == EOF) exit(0);
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
        if ((ch = fgetc(stdin)) == EOF) exit(0);
    }
    long double string_addy = \
        (long double)((unsigned long int)&string_pad + string_start);
    long double string_size = (long double)(string_here - string_start);
    if (def_mode) {
        prog[iptr].function.with_param = push;
        prog[iptr++].param = string_addy;
        prog[iptr].function.with_param = push;
        prog[iptr++].param = string_size; 
    } else {
        push(string_addy);
        push(string_size);
    }
}

static void printfunc()
{
    if (data_stack_ptr < 2) {
        printf("stack underflow! ");
        return;
    }
    unsigned int str_len = (unsigned int) pop();
    char *str_start = (char *)((unsigned long int) pop());
    char dest[str_len + 1];
    char nullstr[] = "\0";
    memcpy(dest, (char *)(str_start + 1), str_len);
    memcpy(dest + str_len, (char *)nullstr, 1);
    printf("%s", dest);
}

/* utf-8 char buffer */
char utf8_buf[5];

static int utf8_encode(char *out, uint32_t utf)
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
    out[1] = (char) (((utf >>  6) & 0x3F) | 0x80);
    out[2] = (char) (((utf >>  0) & 0x3F) | 0x80);
    out[3] = 0;
    return 3;
  }
  else if (utf <= 0x10FFFF) {
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

static void emitfunc()
{
    long unsigned int char_code = (long unsigned int) pop();
    int ulen = utf8_encode(utf8_buf, char_code);
    printf("%s", utf8_buf);
}
