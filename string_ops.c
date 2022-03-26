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

int get_unicode_by_hex(char *chbuf, int usize)
{
    char numstr[usize];
    long int status = (long int) fgets(numstr, usize, ifp);
    int ucode = strtol(numstr, NULL, 16);
    int num_bytes_ret = utf8_encode(chbuf, ucode);
    return status;
}

static void stringfunc()
{
    char ch;
    char escape_ch;
    char chbuf[5];
    int stat;
    DCLANG_UINT chr_cnt = 0;
    DCLANG_UINT bufsize = 32;
    char *scratch = (char *) malloc(sizeof(char) * bufsize);
    // ZERO OUT buffer:
    memset(scratch, 0, bufsize);
    // get the next character, and start the process for real:
    if ((ch = fgetc(ifp)) == EOF) exit(0);
    while (! strchr("\"", ch))
    {
        if (strchr("\\", ch))
        {
            /* consume an extra char due to backslash */
            if ((escape_ch = fgetc(ifp)) == EOF) exit(0);
            switch(escape_ch)
            {
                /* backspace */
                case 'b' :
                  chbuf[0] = 8;
                  chbuf[1] = 0;
                  break;
                /* tab */
                case 't' :
                    chbuf[0] = 9;
                    chbuf[1] = 0;
                    break;
                /* newline
                (line-feed and carriage return together on unix)
                */
                case 'n' :
                    chbuf[0] = 10;
                    chbuf[1] = 0;
                    break;
                /* carriage return */
                case 'r' :
                    chbuf[0] = 13;
                    chbuf[1] = 0;
                    break;
                /* 2-byte unicode */
                case 'u' :
                    stat = get_unicode_by_hex(chbuf, 5);
                    if (stat == 0)
                    {
                        printf("Illegal 2-byte unicode entry in string.\n");
                        return;
                    }
                    break;
                /* 4-byte unicode */
                case 'U' :
                    stat = get_unicode_by_hex(chbuf, 9);
                    if (stat == 0)
                    {
                        printf("Illegal 4-byte unicode entry in string.\n");
                    }
                    break;
                default :
                    chbuf[0] = escape_ch;
                    chbuf[1] = 0;
            }
        } else {
            chbuf[0] = ch;
            chbuf[1] = 0;
        }
        chr_cnt += strlen(chbuf);
        if (chr_cnt > bufsize)
        {
            bufsize += 32;
            scratch = realloc(scratch, bufsize);
        }
        strcat(scratch, chbuf);
        if ((ch = fgetc(ifp)) == EOF) exit(0);
    }
    scratch = realloc(scratch, chr_cnt + 1);
    // Number for stack needs to be a double.
    // Notice the use of "advance" to go beyond any buffer
    // overflow garbage that might appear when the initial
    // buffer is allocated.
    DCLANG_UINT string_dest_uint = (DCLANG_UINT) scratch;
    DCLANG_UINT buflen = (DCLANG_UINT) strlen(scratch);
    if (string_dest_uint < MIN_STR || MIN_STR == 0)
    {
        MIN_STR = string_dest_uint;
    }
    if (string_dest_uint + buflen > MAX_STR || MAX_STR == 0)
    {
        MAX_STR = string_dest_uint + buflen;
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
    DCLANG_UINT string_uint_addr = (DCLANG_UINT) dclang_pop();
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
    fprintf(ofp, "%s", (char *) string_uint_addr);
    fflush(ofp);
}

static void mkbuffunc()
{
    if (data_stack_ptr < 1)
    {
        printf("Stack_underflow!\n");
        printf("'mkbuf' needs <size-as-integer> on the stack\n");
    }
    DCLANG_UINT size = (DCLANG_UINT) dclang_pop();
    char *buf = (char *) malloc(size);
    memset(buf, 0, size);
    int advance = strlen(buf);
    DCLANG_UINT bufaddr = (DCLANG_UINT) buf;
    bufaddr += advance;
    // update print safety:
    if (bufaddr < MIN_STR || MIN_STR == 0)
    {
        MIN_STR = bufaddr;
    }
    if (bufaddr + size + 1 > MAX_STR || MAX_STR == 0)
    {
        MAX_STR = bufaddr + size + 1;
    }
    // done updating
    push(bufaddr);
}

static void freefunc()
{
    if (data_stack_ptr < 1)
    {
        printf("free -- stack underflow! ");
        return;
    }
    DCLANG_UINT loc_uint = (DCLANG_UINT) dclang_pop();
    free((char *) loc_uint);
}

static void emitfunc()
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

static void uemitfunc()
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

static void ordfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("ord -- stack underflow! ");
        return;
    }
    char *string_loc = (char *)(DCLANG_UINT) dclang_pop();
    push((int) *string_loc);
}

static void tohexfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("tohex -- stack underflow! ");
        return;
    }
    DCLANG_INT val = (DCLANG_INT) dclang_pop();
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
    DCLANG_UINT string_uint_addr = (DCLANG_UINT) dclang_pop();
    if (string_uint_addr < MIN_STR || string_uint_addr > MAX_STR)
    {
        perror("tonum -- String address out-of-range.");
        return;
    }
    char *mystr = (char *) string_uint_addr;
    DCLANG_FLT num = strtod(mystr, NULL);
    push(num);
}

static void tostrfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("tostr -- needs a number on stack! ");
        return;
    }
    DCLANG_FLT var = dclang_pop();
    int bufsize = snprintf( NULL, 0, "%g", var);
    char *str = malloc(bufsize + 1);
    snprintf(str, bufsize + 1, "%g", var);
    DCLANG_UINT string_uint_addr = (DCLANG_UINT) str;
    if (string_uint_addr < MIN_STR || MIN_STR == 0)
    {
        MIN_STR = string_uint_addr;
    }
    if (string_uint_addr + bufsize + 1 > MAX_STR || MAX_STR == 0)
    {
        MAX_STR = string_uint_addr + bufsize + 1;
    }
    push((DCLANG_UINT) string_uint_addr);
}

static void strlenfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("strlen -- stack underflow! ");
        return;
    }
    DCLANG_UINT string_uint_addr = (DCLANG_UINT) dclang_pop();
    if (string_uint_addr < MIN_STR || string_uint_addr > MAX_STR)
    {
        perror("strlen -- String address out-of-range.");
        return;
    }
    char *mystr = (char *) string_uint_addr;
    push((DCLANG_UINT) strlen(mystr));
}

static void streqfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("str= -- stack underflow! ");
        return;
    }
    DCLANG_UINT string_uint_addr2 = (DCLANG_UINT) dclang_pop();
    DCLANG_UINT string_uint_addr1 = (DCLANG_UINT) dclang_pop();
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
    push(((DCLANG_INT) strcmp(str1, str2) == 0) * -1);
}

static void strltfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("str< -- stack underflow! ");
        return;
    }
    DCLANG_UINT string_uint_addr2 = (DCLANG_UINT) dclang_pop();
    DCLANG_UINT string_uint_addr1 = (DCLANG_UINT) dclang_pop();
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
    push(((DCLANG_INT) strcmp(str1, str2) < 0) * -1);
}

static void strgtfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("str> -- stack underflow! ");
        return;
    }
    DCLANG_UINT string_uint_addr2 = (DCLANG_UINT) dclang_pop();
    DCLANG_UINT string_uint_addr1 = (DCLANG_UINT) dclang_pop();
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
    push(((DCLANG_INT) strcmp(str1, str2) > 0) * -1);
}

static void strfindfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("strfind -- needs <haystack> <needle> string pointers on stack! ");
        return;
    }
    DCLANG_UINT string_uint_addr2 = (DCLANG_UINT) dclang_pop();
    DCLANG_UINT string_uint_addr1 = (DCLANG_UINT) dclang_pop();
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
    push((DCLANG_INT) strstr(str1, str2));
}


static void strtokfunc()
{
    if (data_stack_ptr < 3)
    {
        printf("strtok -- needs <str> <delim> <saveptr> string pointers on stack!\n");
        printf("<saveptr> should be a variable slot declared with `var`, without being dereferenced with `@`.\n");
        printf("e.g.:\n\nvar mysave\n\"split.this.string!\" \".\" mysave strtok print cr\n");
        printf("split\nnull \".\" mysave strtok print cr\n");
        printf("this\nnull \".\" mysave strtok print cr\n");
        printf("string!\n\n");
        return;
    }
    DCLANG_UINT savepoint = (DCLANG_UINT) dclang_pop();
    char **savepoint_ptr = (char **) &vars[savepoint];
    DCLANG_UINT string_uint_addr2 = (DCLANG_UINT) dclang_pop();
    DCLANG_UINT string_uint_addr1 = (DCLANG_UINT) dclang_pop();
    if ((string_uint_addr1 != 0) && (string_uint_addr1 < MIN_STR || string_uint_addr1 > MAX_STR))
    {
        perror("strtok -- <str> (first) string address out-of-range.");
        return;
    }
    if (string_uint_addr2 < MIN_STR || string_uint_addr2 > MAX_STR)
    {
        perror("strtok -- <delim> (second) string address out-of-range.");
        return;
    }
    char *str1 = (char *) string_uint_addr1;
    char *str2 = (char *) string_uint_addr2;
    push((DCLANG_INT) strtok_r(str1, str2, savepoint_ptr));
}


static void mempcpyfunc()
{
    if (data_stack_ptr < 3)
    {
        printf("mempcpy -- needs <dest> <source> <size> on stack! ");
        return;
    }
    DCLANG_UINT size = (DCLANG_UINT) dclang_pop();
    DCLANG_UINT source = (DCLANG_UINT) dclang_pop();
    DCLANG_UINT dest = (DCLANG_UINT) dclang_pop();
    if ((dest != 0) && (dest < MIN_STR || dest > MAX_STR))
    {
        perror("mempcpy -- <dest> string address out-of-range.");
        return;
    }
    if (source < MIN_STR || source > MAX_STR)
    {
        perror("mempcpy -- <source> string address out-of-range.");
        return;
    }
    push((DCLANG_UINT) mempcpy((char *)dest, (char *)source, (DCLANG_UINT) size));
}

static void memsetfunc()
{
    if (data_stack_ptr < 3)
    {
        printf("memset -- needs <dest_str> <char-int> <times-int> on stack! ");
        return;
    }
    DCLANG_UINT times = (DCLANG_UINT) dclang_pop();
    DCLANG_UINT chr = (DCLANG_UINT) dclang_pop();
    DCLANG_UINT dest = (DCLANG_UINT) dclang_pop();
    if ((dest != 0) && (dest < MIN_STR || dest > MAX_STR))
    {
        perror("memset -- <dest> string address out-of-range.");
        return;
    }
    push((DCLANG_INT) memset((char *)dest, (int)chr, (int)times));
}

static void bytes32func()
{
    DCLANG_INT32 val = (DCLANG_INT32) dclang_pop();
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
