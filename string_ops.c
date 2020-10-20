MYUINT MIN_STR = 0;
MYUINT MAX_STR = 0;

/* utf-8 char buffer */
char utf8_buf[5];

static long utf8_encode(char *out, uint64_t utf)
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

int get_unicode_by_hex(char *c, int usize)
{
    char numstr[usize];
    long int status = (long int)fgets(numstr, usize, ifp);
    int ucode = strtol(numstr, NULL, 16);
    int num_bytes_ret = utf8_encode(c, ucode);
    return status;
}

static void stringfunc()
{
    char ch;
    char chbuf[5];
    char *buf;
    MYUINT bufsize_step = 64;
    MYUINT cur_bufsize = 0;
    MYUINT bufsize = bufsize_step;
    buf = (char *)malloc(bufsize);
    // get the next character, and start the process for real:
    if ((ch = fgetc(ifp)) == EOF) exit(0);
    while (! strchr("\"", ch))
    {
        if (strchr("\\", ch))
        {
            /* consume an extra char due to backslash */
            if ((ch = fgetc(ifp)) == EOF) exit(0);
            /* backspace */
            if (strchr("b", ch))
            {
                chbuf[0] = 8;
                chbuf[1] = 0;
            }
            /* tab */
            if (strchr("t", ch))
            {
                chbuf[0] = 9;
                chbuf[1] = 0;
            }
            /* newline (line-feed and carriage return together on unix) */
            if (strchr("n", ch))
            {
                chbuf[0] = 10;
                chbuf[1] = 0;
            }
            /* carriage return */
            if (strchr("r", ch))
            {
                chbuf[0] = 13;
                chbuf[1] = 0;
            }
            /* 2-byte unicode */
            if (strchr("u", ch))
            {
                int stat = get_unicode_by_hex(chbuf, 5);
                if (stat == 0)
                {
                    printf("Illegal 2-byte unicode entry in string.\n");
                    return;
                }
            }
            /* 4-byte unicode */
            if (strchr("U", ch))
            {
                int stat = get_unicode_by_hex(chbuf, 9);
                if (stat == 0)
                {
                    printf("Illegal 4-byte unicode entry in string.\n");
                }
            }
        } else {
            chbuf[0] = ch;
            chbuf[1] = 0;
        }
        cur_bufsize += strlen(chbuf);
        if (cur_bufsize > bufsize)
        {
            bufsize += bufsize_step;
            buf = (char *) realloc(buf, bufsize);
        }
        strcat(buf, chbuf);
        if ((ch = fgetc(ifp)) == EOF) exit(0);
    }
    // number for stack needs to be a double:
    MYUINT string_dest_uint = (MYUINT) buf;
    if (string_dest_uint < MIN_STR || MIN_STR == 0)
    {
        MIN_STR = string_dest_uint;
    }
    if (string_dest_uint + bufsize + 1 > MAX_STR || MAX_STR == 0)
    {
        MAX_STR = string_dest_uint + bufsize + 1;
    }
    if (def_mode)
    {
        prog[iptr].function.with_param = push;
        prog[iptr++].param = string_dest_uint;
    } else {
        push(string_dest_uint);
    }
}

static void printfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("print -- stack underflow! ");
        return;
    }
    MYUINT string_uint_addr = (MYUINT) pop();
    if (string_uint_addr == 0)
    {
        printf("print -- Nothing to print.");
        return;
    }
    if (string_uint_addr < MIN_STR || string_uint_addr > MAX_STR)
    {
        perror("print -- String address out-of-range.");
        return;
    }
    fprintf(ofp, "%s", (char *)string_uint_addr);
    fflush(ofp);
}

static void freefunc()
{
    if (data_stack_ptr < 1)
    {
        printf("free -- stack underflow! ");
        return;
    }
    char *string_loc = (char *)(MYUINT) pop();
    free(string_loc);
}

static void emitfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("emit -- stack underflow! ");
        return;
    }
    char char_code = (char) pop();
    fprintf(ofp, "%c", char_code);
    fflush(ofp);
}

static void uemitfunc()
{
    if (data_stack_ptr < 1)
    {
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
    if (data_stack_ptr < 1)
    {
        printf("ord -- stack underflow! ");
        return;
    }
    char *string_loc = (char *)(MYUINT) pop();
    push((int) *string_loc);
}

static void tohexfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("tohex -- stack underflow! ");
        return;
    }
    MYINT val = (MYINT) pop();
    fprintf(ofp, "0x%.2lx", val);
    fflush(ofp);
}

static void tonumfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("tonum -- needs a <str-pointer> on stack! ");
        return;
    }
    MYUINT string_uint_addr = (MYUINT) pop();
    if (string_uint_addr < MIN_STR || string_uint_addr > MAX_STR)
    {
        perror("tonum -- String address out-of-range.");
        return;
    }
    char *mystr = (char *) string_uint_addr;
    MYFLT num = strtod(mystr, NULL);
    push(num);
}

static void tostrfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("tostr -- needs a number on stack! ");
        return;
    }
    MYFLT var = pop();
    int bufsize = snprintf( NULL, 0, "%g", var);
    char *str = malloc(bufsize + 1);
    snprintf(str, bufsize + 1, "%g", var);
    MYUINT string_uint_addr = (MYUINT) str;
    if (string_uint_addr < MIN_STR || MIN_STR == 0)
    {
        MIN_STR = string_uint_addr;
    }
    if (string_uint_addr + bufsize + 1 > MAX_STR || MAX_STR == 0)
    {
        MAX_STR = string_uint_addr + bufsize + 1;
    }
    push((MYUINT) string_uint_addr);
}

static void strlenfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("strlen -- stack underflow! ");
        return;
    }
    MYUINT string_uint_addr = (MYUINT) pop();
    if (string_uint_addr < MIN_STR || string_uint_addr > MAX_STR)
    {
        perror("strlen -- String address out-of-range.");
        return;
    }
    char *mystr = (char *) string_uint_addr;
    push((MYUINT) strlen(mystr));
}

static void streqfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("str= -- stack underflow! ");
        return;
    }
    MYUINT string_uint_addr2 = (MYUINT) pop();
    MYUINT string_uint_addr1 = (MYUINT) pop();
    if (string_uint_addr1 < MIN_STR || string_uint_addr1 > MAX_STR)
    {
        perror("strlen -- First given string address out-of-range.");
        return;
    }
    if (string_uint_addr2 < MIN_STR || string_uint_addr2 > MAX_STR)
    {
        perror("strlen -- Second given string address out-of-range.");
        return;
    }
    char *str1 = (char *) string_uint_addr1;
    char *str2 = (char *) string_uint_addr2;
    push((MYINT) strcmp(str1, str2) == 0);
}

static void strltfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("str< -- stack underflow! ");
        return;
    }
    MYUINT string_uint_addr2 = (MYUINT) pop();
    MYUINT string_uint_addr1 = (MYUINT) pop();
    if (string_uint_addr1 < MIN_STR || string_uint_addr1 > MAX_STR)
    {
        perror("str< -- First given string address out-of-range.");
        return;
    }
    if (string_uint_addr2 < MIN_STR || string_uint_addr2 > MAX_STR)
    {
        perror("str< -- Second given string address out-of-range.");
        return;
    }
    char *str1 = (char *) string_uint_addr1;
    char *str2 = (char *) string_uint_addr2;
    push((MYINT) strcmp(str1, str2) < 0);
}

static void strgtfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("str> -- stack underflow! ");
        return;
    }
    MYUINT string_uint_addr2 = (MYUINT) pop();
    MYUINT string_uint_addr1 = (MYUINT) pop();
    if (string_uint_addr1 < MIN_STR || string_uint_addr1 > MAX_STR)
    {
        perror("str> -- First given string address out-of-range.");
        return;
    }
    if (string_uint_addr2 < MIN_STR || string_uint_addr2 > MAX_STR)
    {
        perror("str> -- Second given string address out-of-range.");
        return;
    }
    char *str1 = (char *) string_uint_addr1;
    char *str2 = (char *) string_uint_addr2;
    push((MYINT) strcmp(str1, str2) > 0);
}

static void strfindfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("strfind -- needs <haystack> <needle> string pointers on stack! ");
        return;
    }
    MYUINT string_uint_addr2 = (MYUINT) pop();
    MYUINT string_uint_addr1 = (MYUINT) pop();
    if (string_uint_addr1 < MIN_STR || string_uint_addr1 > MAX_STR)
    {
        perror("strfind -- 'haystack' (first) string address out-of-range.");
        return;
    }
    if (string_uint_addr2 < MIN_STR || string_uint_addr2 > MAX_STR)
    {
        perror("strfind -- 'needle' (second) string address out-of-range.");
        return;
    }
    char *str1 = (char *) string_uint_addr1;
    char *str2 = (char *) string_uint_addr2;
    push((MYINT) strstr(str1, str2));
}

// destructive/creative string functions:
static void strcatfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("strcat -- needs <dest> <source> string pointers on stack! ");
        return;
    }
    MYUINT string_uint_addr2 = (MYUINT) pop();
    MYUINT string_uint_addr1 = (MYUINT) pop();
    if (string_uint_addr1 < MIN_STR || string_uint_addr1 > MAX_STR)
    {
        perror("strcat --  <dest> (first) string address out-of-range.");
        return;
    }
    if (string_uint_addr2 < MIN_STR || string_uint_addr2 > MAX_STR)
    {
        perror("strcat -- <source> (second) string address out-of-range.");
        return;
    }
    char *str1 = (char *) string_uint_addr1;
    char *str2 = (char *) string_uint_addr2;
    push((MYINT) strcat(str1, str2));
}

static void strcpyfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("strcpy -- needs <dest> <source> string pointers on stack! ");
        return;
    }
    MYUINT string_uint_addr2 = (MYUINT) pop();
    MYUINT string_uint_addr1 = (MYUINT) pop();
    if (string_uint_addr1 < MIN_STR || string_uint_addr1 > MAX_STR)
    {
        perror("strcpy --  <dest> (first) string address out-of-range.");
        return;
    }
    if (string_uint_addr2 < MIN_STR || string_uint_addr2 > MAX_STR)
    {
        perror("strcpy -- <source> (second) string address out-of-range.");
        return;
    }
    char *str1 = (char *) string_uint_addr1;
    char *str2 = (char *) string_uint_addr2;
    push((MYINT) strcpy(str1, str2));
}

static void strdupfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("strdup -- needs <string> string pointer on stack! ");
        return;
    }
    MYUINT string_uint_addr = (MYUINT) pop();
    if (string_uint_addr < MIN_STR || string_uint_addr > MAX_STR)
    {
        perror("strdup -- string address out-of-range.");
        return;
    }
    char *str1 = strdup((char *) string_uint_addr);
    MYUINT str1addr = (MYUINT) str1;
    if (str1addr < MIN_STR || MIN_STR == 0)
    {
        MIN_STR = str1addr;
    }
    if (str1addr > MAX_STR || MAX_STR == 0)
    {
        MAX_STR = str1addr;
    }
    push((MYUINT) str1);
}

static void strtokfunc()
{
    if (data_stack_ptr < 3)
    {
        printf("strtok -- needs <str> <delim> <saveptr> string pointers on stack!\n");
        printf("<saveptr> should be a variable slot declared with `var`, without being dereferenced with `@`.\n");
        printf("e.g. var mysavepoint strok_r \"split.this.string\" \".\" mysavepoint strtok_r\n");
        return;
    }
    MYUINT savepoint = (MYUINT) pop();
    char **savepoint_ptr = (char **) &myvars[savepoint];
    MYUINT string_uint_addr2 = (MYUINT) pop();
    MYUINT string_uint_addr1 = (MYUINT) pop();
    if ((string_uint_addr1 != 0) && (string_uint_addr1 < MIN_STR || string_uint_addr1 > MAX_STR))
    {
        perror("strtok --  <str> (first) string address out-of-range.");
        return;
    }
    if (string_uint_addr2 < MIN_STR || string_uint_addr2 > MAX_STR)
    {
        perror("strtok -- <delim> (second) string address out-of-range.");
        return;
    }
    char *str1 = (char *) string_uint_addr1;
    char *str2 = (char *) string_uint_addr2;
    push((MYINT) strtok_r(str1, str2, savepoint_ptr));
}

static void memcpyfunc()
{
    if (data_stack_ptr < 3)
    {
        printf("memcpy -- needs <dest> <source> <size> on stack! ");
        return;
    }
    MYUINT size = (MYUINT) pop();
    MYUINT source = (MYUINT) pop();
    MYUINT dest = (MYUINT) pop();
    if ((dest != 0) && (dest < MIN_STR || dest > MAX_STR))
    {
        perror("memcpy --  <dest> string address out-of-range.");
        return;
    }
    if (source < MIN_STR || source > MAX_STR)
    {
        perror("memcpy -- <source> string address out-of-range.");
        return;
    }
    push((MYUINT) memcpy((char *)dest, (char *)source, (MYUINT) size));
}

static void mempcpyfunc()
{
    if (data_stack_ptr < 3)
    {
        printf("mempcpy -- needs <dest> <source> <size> on stack! ");
        return;
    }
    MYUINT size = (MYUINT) pop();
    MYUINT source = (MYUINT) pop();
    MYUINT dest = (MYUINT) pop();
    if ((dest != 0) && (dest < MIN_STR || dest > MAX_STR))
    {
        perror("mempcpy --  <dest> string address out-of-range.");
        return;
    }
    if (source < MIN_STR || source > MAX_STR)
    {
        perror("mempcpy -- <source> string address out-of-range.");
        return;
    }
    push((MYUINT) mempcpy((char *)dest, (char *)source, (MYUINT) size));
}

static void memsetfunc()
{
    if (data_stack_ptr < 3)
    {
        printf("memset -- needs <dest_str> <char-int> <times-int> on stack! ");
        return;
    }
    MYUINT times = (MYUINT) pop();
    MYUINT chr = (MYUINT) pop();
    MYUINT dest = (MYUINT) pop();
    if ((dest != 0) && (dest < MIN_STR || dest > MAX_STR))
    {
        perror("memset --  <dest> string address out-of-range.");
        return;
    }
    push((MYINT) memset((char *)dest, (int)chr, (int)times));
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
