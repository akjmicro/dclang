char string_pad[1048576];
unsigned long string_here;

MYUINT MIN_STR = 0;
MYUINT MAX_STR = 0;

static void stringfunc()
{
    long ch;
    /* get a starting marker for length */
    unsigned long string_start = string_here;
    // get the next character, and start the process for real:
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
    unsigned long string_addr = (unsigned long) string_start;
    unsigned long string_size = (unsigned long)(string_here - string_start);
    char *string_dest = malloc(string_size + 1);
    // number for stack needs to be a double:
    MYUINT string_dest_uint = (MYUINT) string_dest;
    if (string_dest_uint < MIN_STR || MIN_STR == 0) {
        MIN_STR = string_dest_uint;
    }
    if (string_dest_uint + string_size + 1 > MAX_STR || MAX_STR == 0) {
        MAX_STR = string_dest_uint + string_size + 1;
    }
    char nullstr[] = "\0";
    memcpy(string_dest, (char *)((MYUINT)&string_pad[0] + string_addr), string_size);
    if (def_mode) {
        prog[iptr].function.with_param = push;
        prog[iptr++].param = string_dest_uint;
    } else {
        push(string_dest_uint);
    }
}

static void printfunc()
{
    if (data_stack_ptr < 1) {
        printf("print -- stack underflow! ");
        return;
    }
    MYUINT string_dest = (MYUINT) pop();
    if (string_dest == 0) {
        printf("print -- Nothing to print.");
        return;
    }
    if (string_dest < MIN_STR || string_dest > MAX_STR) {
        perror("print -- String address out-of-range.");
        return;
    }
    fprintf(ofp, "%s", (char *)string_dest);
    fflush(ofp);
}

static void freefunc()
{
    if (data_stack_ptr < 1) {
        printf("free -- stack underflow! ");
        return;
    }
    char *string_loc = (char *)(MYUINT) pop();
    free(string_loc);
}

static void emitfunc()
{
    if (data_stack_ptr < 1) {
        printf("emit -- stack underflow! ");
        return;
    }
    char char_code = (char) pop();
    fprintf(ofp, "%c", char_code);
    fflush(ofp);
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

static void uemitfunc()
{
    if (data_stack_ptr < 1) {
        printf("uemit -- stack underflow! ");
        return;
    }
    long unsigned long char_code = (long unsigned long) pop();
    long ulen = utf8_encode(utf8_buf, char_code);
    fprintf(ofp, "%s", utf8_buf);
    fflush(ofp);
}

static void ordfunc()
{
    if (data_stack_ptr < 1) {
        printf("ord -- stack underflow! ");
        return;
    }
    char *string_loc = (char *)(MYUINT) pop();
    push((int) *string_loc);
}

static void tohexfunc()
{
    if (data_stack_ptr < 1) {
        printf("tohex -- stack underflow! ");
        return;
    }
    MYINT val = (MYINT) pop();
    fprintf(ofp, "0x%.2lx", val);
    fflush(ofp);
}

static void strlenfunc()
{
    if (data_stack_ptr < 1) {
        printf("strlen -- stack underflow! ");
        return;
    }
    MYUINT val = (MYUINT) pop();
    push((MYUINT) strlen((char *)val));
}

static void streqfunc()
{
    if (data_stack_ptr < 2) {
        printf("str= -- stack underflow! ");
        return;
    }
    MYUINT str2 = (MYUINT) pop();
    MYUINT str1 = (MYUINT) pop();
    push((MYINT) strcmp((char *)str1, (char *)str2) == 0);
}

static void strltfunc()
{
    if (data_stack_ptr < 2) {
        printf("str< -- stack underflow! ");
        return;
    }
    MYUINT str2 = (MYUINT) pop();
    MYUINT str1 = (MYUINT) pop();
    push((MYINT) strcmp((char *)str1, (char *)str2) < 0);
}

static void strgtfunc()
{
    if (data_stack_ptr < 2) {
        printf("str> -- stack underflow! ");
        return;
    }
    MYUINT str2 = (MYUINT) pop();
    MYUINT str1 = (MYUINT) pop();
    push((MYINT) strcmp((char *)str1, (char *)str2) > 0);
}

static void strfindfunc()
{
    if (data_stack_ptr < 2) {
        printf("strfind -- stack underflow! ");
        return;
    }
    MYUINT str2 = (MYUINT) pop();
    MYUINT str1 = (MYUINT) pop();
    push((MYINT) strstr((char *)str1, (char *)str2));
}

static void bytes32func()
{
    MYINT32 val = (MYINT32) pop();
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
