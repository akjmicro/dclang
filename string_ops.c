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

void stringfunc()
{
    char ch;
    char escape_ch;
    char chbuf[5];
    int stat;
    DCLANG_UINT chr_cnt = 0;
    DCLANG_UINT bufsize = 2048;
    char *scratch = (char *) calloc(bufsize, 1);
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
                /* 1-byte ASCII code */
                case 'x' :
                    stat = get_ascii(chbuf, 3);
                    if (stat == 0)
                    {
                        printf("Illegal 1-byte ASCII string denoted with \\x.\n");
                        return;
                    }
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
            bufsize *= 2;
            // Alpine Linux (musl) seems to not like `realloc`, causing crashes.
            // So, the solution here is a homemade `realloc`!
            char *new_scratch = (char *) calloc(bufsize, 1);
            if (new_scratch != NULL) {
                memcpy(new_scratch, scratch, strlen(scratch));
                free(scratch);
                scratch = new_scratch;
            } else {
                printf("Re-allocation of memory failed during string gulping. Exiting.\n");
                exit(1);
            }
        }
        strcat(scratch, chbuf);
        if ((ch = fgetc(ifp)) == EOF) exit(0);
    }
    // copy the scratch buf to the final location
    char *final = calloc(chr_cnt + 1, 1);
    memcpy(final, scratch, chr_cnt);
    free(scratch);
    // register the string with MIN_STR and MAX_STR
    DCLANG_PTR string_dest_ptr = (DCLANG_PTR) final;
    DCLANG_PTR buflen = (DCLANG_PTR) chr_cnt;
    if (string_dest_ptr < MIN_STR || MIN_STR == 0)
    {
        MIN_STR = string_dest_ptr;
    }
    if (string_dest_ptr + buflen > MAX_STR || MAX_STR == 0)
    {
        MAX_STR = string_dest_ptr + buflen;
    }
    // Do the right thing depending on def_mode
    if (def_mode)
    {
        prog[iptr].function.with_param = push;
        prog[iptr++].param = string_dest_ptr;
    } else {
        push(string_dest_ptr);
    }
}

void printfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("print -- stack underflow! ");
        return;
    }
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr == 0)
    {
        printf("print -- Nothing to print.");
        return;
    }
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("print -- String address out-of-range.");
        return;
    }
    fprintf(ofp, "%s", (char *) string_PTR_addr);
    fflush(ofp);
}

void mkbuffunc()
{
    if (data_stack_ptr < 1)
    {
        printf("Stack_underflow!\n");
        printf("'mkbuf' needs <size-as-integer> on the stack\n");
    }
    DCLANG_PTR size = (DCLANG_PTR) dclang_pop();
    char *buf = (char *) calloc(size, 1);
    memset(buf, 0, size);
    DCLANG_PTR advance = (DCLANG_PTR) strlen(buf);
    DCLANG_PTR bufaddr = (DCLANG_PTR) buf;
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

void freefunc()
{
    if (data_stack_ptr < 1)
    {
        printf("free -- stack underflow! ");
        return;
    }
    DCLANG_PTR loc_PTR = (DCLANG_PTR) dclang_pop();
    free((char *) loc_PTR);
}

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

void ordfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("ord -- stack underflow! ");
        return;
    }
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("ord -- String address out-of-range.");
        return;
    }
    char *string_loc = (char *) string_PTR_addr;
    push((int) *string_loc);
}

void tohexfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("tohex -- stack underflow! ");
        return;
    }
    DCLANG_INT val = (DCLANG_INT) dclang_pop();
    int bufsize = snprintf(NULL, 0, "0x%.2lx", val);
    char *str = calloc(bufsize + 1, 1);
    snprintf(str, bufsize + 1, "0x%.2lx", val);
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) str;
    if (string_PTR_addr < MIN_STR || MIN_STR == 0)
    {
        MIN_STR = string_PTR_addr;
    }
    if (string_PTR_addr + bufsize + 1 > MAX_STR || MAX_STR == 0)
    {
        MAX_STR = string_PTR_addr + bufsize + 1;
    }
    push((DCLANG_PTR) string_PTR_addr);
}

void tonumfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("tonum -- needs a <str-pointer> on stack! ");
        return;
    }
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("tonum -- String address out-of-range.");
        return;
    }
    char *mystr = (char *) string_PTR_addr;
    DCLANG_FLT num = strtod(mystr, NULL);
    push(num);
}

void tostrfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("tostr -- needs a number on stack! ");
        return;
    }
    DCLANG_FLT var = dclang_pop();
    int bufsize = snprintf(NULL, 0, "%g", var);
    char *str = calloc(bufsize + 1, 1);
    snprintf(str, bufsize + 1, "%g", var);
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) str;
    if (string_PTR_addr < MIN_STR || MIN_STR == 0)
    {
        MIN_STR = string_PTR_addr;
    }
    if (string_PTR_addr + bufsize + 1 > MAX_STR || MAX_STR == 0)
    {
        MAX_STR = string_PTR_addr + bufsize + 1;
    }
    push((DCLANG_PTR) string_PTR_addr);
}

void strlenfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("strlen -- stack underflow! ");
        return;
    }
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("strlen -- String address out-of-range.");
        return;
    }
    char *mystr = (char *) string_PTR_addr;
    push((DCLANG_PTR) strlen(mystr));
}

void streqfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("str= -- stack underflow! ");
        return;
    }
    DCLANG_PTR string_PTR_addr2 = (DCLANG_PTR) dclang_pop();
    DCLANG_PTR string_PTR_addr1 = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr1 < MIN_STR || string_PTR_addr1 > MAX_STR)
    {
        perror("strlen -- First given string address out-of-range.");
        return;
    }
    if (string_PTR_addr2 < MIN_STR || string_PTR_addr2 > MAX_STR)
    {
        perror("strlen -- Second given string address out-of-range.");
        return;
    }
    char *str1 = (char *) string_PTR_addr1;
    char *str2 = (char *) string_PTR_addr2;
    push(((DCLANG_INT) strcmp(str1, str2) == 0) * -1);
}

void strltfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("str< -- stack underflow! ");
        return;
    }
    DCLANG_PTR string_PTR_addr2 = (DCLANG_PTR) dclang_pop();
    DCLANG_PTR string_PTR_addr1 = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr1 < MIN_STR || string_PTR_addr1 > MAX_STR)
    {
        perror("str< -- First given string address out-of-range.");
        return;
    }
    if (string_PTR_addr2 < MIN_STR || string_PTR_addr2 > MAX_STR)
    {
        perror("str< -- Second given string address out-of-range.");
        return;
    }
    char *str1 = (char *) string_PTR_addr1;
    char *str2 = (char *) string_PTR_addr2;
    push(((DCLANG_INT) strcmp(str1, str2) < 0) * -1);
}

void strgtfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("str> -- stack underflow! ");
        return;
    }
    DCLANG_PTR string_PTR_addr2 = (DCLANG_PTR) dclang_pop();
    DCLANG_PTR string_PTR_addr1 = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr1 < MIN_STR || string_PTR_addr1 > MAX_STR)
    {
        perror("str> -- First given string address out-of-range.");
        return;
    }
    if (string_PTR_addr2 < MIN_STR || string_PTR_addr2 > MAX_STR)
    {
        perror("str> -- Second given string address out-of-range.");
        return;
    }
    char *str1 = (char *) string_PTR_addr1;
    char *str2 = (char *) string_PTR_addr2;
    push(((DCLANG_INT) strcmp(str1, str2) > 0) * -1);
}

void strfindfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("strfind -- needs <haystack> <needle> string pointers on stack! ");
        return;
    }
    DCLANG_PTR string_PTR_addr2 = (DCLANG_PTR) dclang_pop();
    DCLANG_PTR string_PTR_addr1 = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr1 < MIN_STR || string_PTR_addr1 > MAX_STR)
    {
        perror("strfind -- 'haystack' (first) string address out-of-range.");
        return;
    }
    if (string_PTR_addr2 < MIN_STR || string_PTR_addr2 > MAX_STR)
    {
        perror("strfind -- 'needle' (second) string address out-of-range.");
        return;
    }
    char *str1 = (char *) string_PTR_addr1;
    char *str2 = (char *) string_PTR_addr2;
    push((DCLANG_INT) strstr(str1, str2));
}

void strspnfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("strspn -- needs <str> <test_chars_str> pointers on the stack!\n");
        return;
    }
    DCLANG_PTR delim = (DCLANG_PTR) dclang_pop();
    DCLANG_PTR str = (DCLANG_PTR) dclang_pop();
    if ((delim != 0) && (delim < MIN_STR || delim > MAX_STR))
    {
        perror("strspn -- <delim> string address out-of-range.");
        return;
    }
    if (str < MIN_STR || str > MAX_STR)
    {
        perror("strspn -- <str> string address out-of-range.");
        return;
    }
    push((DCLANG_INT) strspn((char *)str, (char *)delim));
}

void strcspnfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("strcspn -- needs <str> <test_chars_str> pointers on the stack!\n");
        return;
    }
    DCLANG_PTR delim = (DCLANG_PTR) dclang_pop();
    DCLANG_PTR str = (DCLANG_PTR) dclang_pop();
    if ((delim != 0) && (delim < MIN_STR || delim > MAX_STR))
    {
        perror("strcspn -- <test_chars_str> string address out-of-range.");
        return;
    }
    if (str < MIN_STR || str > MAX_STR)
    {
        perror("strcspn -- <str> string address out-of-range.");
        return;
    }
    push((DCLANG_INT) strcspn((char *)str, (char *)delim));
}

void strtokfunc()
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
    DCLANG_PTR savepoint = (DCLANG_PTR) dclang_pop();
    char **savepoint_ptr = (char **) &vars[savepoint];
    DCLANG_PTR delim = (DCLANG_PTR) dclang_pop();
    DCLANG_PTR str = (DCLANG_PTR) dclang_pop();
    if ((str != 0) && (str < MIN_STR || str > MAX_STR))
    {
        perror("strtok -- <str> (first) string address out-of-range.");
        return;
    }
    if (delim < MIN_STR || delim > MAX_STR)
    {
        perror("strtok -- <delim> (second) string address out-of-range.");
        return;
    }
    push((DCLANG_INT) strtok_r((char *)str, (char *)delim, savepoint_ptr));
}

void mempcpyfunc()
{
    if (data_stack_ptr < 3)
    {
        printf("mempcpy -- needs <dest> <source> <size> on stack! ");
        return;
    }
    DCLANG_PTR size = (DCLANG_PTR) dclang_pop();
    DCLANG_PTR source = (DCLANG_PTR) dclang_pop();
    DCLANG_PTR dest = (DCLANG_PTR) dclang_pop();
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
    push(
        (DCLANG_PTR) memcpy(
        (char *)dest, (char *)source, (DCLANG_PTR) size) + size
    );
}

void memsetfunc()
{
    if (data_stack_ptr < 3)
    {
        printf("memset -- needs <dest_str> <char-int> <times-int> on stack! ");
        return;
    }
    DCLANG_PTR times = (DCLANG_PTR) dclang_pop();
    DCLANG_PTR chr = (DCLANG_PTR) dclang_pop();
    DCLANG_PTR dest = (DCLANG_PTR) dclang_pop();
    if ((dest != 0) && (dest < MIN_STR || dest > MAX_STR))
    {
        perror("memset -- <dest> string address out-of-range.");
        return;
    }
    push((DCLANG_PTR)memset((char *)dest, (int)chr, (int)times));
}

void bytes32func()
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

void tolowerfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("tolower -- needs a <source_str> pointer on the stack! ");
        return;
    }
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("tolower -- String address out-of-range.");
        return;
    }
    char *mystr = (char *) string_PTR_addr;
    DCLANG_PTR buflen = (DCLANG_PTR) strlen(mystr);
    char *buf = (char *) calloc(buflen, 1);
    DCLANG_PTR string_dest_PTR = (DCLANG_PTR) buf;
    int i = 0;
    int c = 0;
    while(*(mystr + i)) {
      c = (int) *(mystr + i);
      memset(buf + i, tolower(c), 1);
      i++;
    }
    if (string_dest_PTR < MIN_STR || MIN_STR == 0)
    {
        MIN_STR = string_dest_PTR;
    }
    if (string_dest_PTR + buflen > MAX_STR || MAX_STR == 0)
    {
        MAX_STR = string_dest_PTR + buflen;
    }
    push((DCLANG_PTR) buf);
}

void toupperfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("toupper -- needs a <source_str> pointer on the stack! ");
        return;
    }
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("toupper -- String address out-of-range.");
        return;
    }
    char *mystr = (char *) string_PTR_addr;
    DCLANG_PTR buflen = (DCLANG_PTR) strlen(mystr);
    char *buf = (char *) calloc(buflen, 1);
    DCLANG_PTR string_dest_PTR = (DCLANG_PTR) buf;
    int i = 0;
    int c = 0;
    while(*(mystr + i)) {
      c = (int) *(mystr + i);
      memset(buf + i, toupper(c), 1);
      i++;
    }
    if (string_dest_PTR < MIN_STR || MIN_STR == 0)
    {
        MIN_STR = string_dest_PTR;
    }
    if (string_dest_PTR + buflen > MAX_STR || MAX_STR == 0)
    {
        MAX_STR = string_dest_PTR + buflen;
    }
    push((DCLANG_PTR) buf);
}
