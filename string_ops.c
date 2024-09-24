void stringfunc()
{
    char ch;
    char escape_ch;
    char chbuf[5];
    int stat;
    char *scratch = &memory_pool[unused_mem_idx];
    char *start_scratch = scratch;
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
        scratch = mempcpy(scratch, chbuf, strlen(chbuf));
        if ((ch = fgetc(ifp)) == EOF) exit(0);
    }
    memset(scratch, 0, 1);
    int chr_cnt = (scratch - start_scratch) + 1;
    unused_mem_idx = (unused_mem_idx + chr_cnt + 0x0f) & ~0x0f;
    // register the string with MIN_STR and MAX_STR
    DCLANG_PTR string_dest_ptr = (DCLANG_PTR) start_scratch;
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
        printf("print -- stack underflow!\n");
        return;
    }
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr == 0)
    {
        printf("print -- Nothing to print.\n");
        return;
    }
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("print -- String address out-of-range.\n");
        return;
    }
    fprintf(ofp, "%s", (char *) string_PTR_addr);
    fflush(ofp);
}

void crfunc()
{
    fprintf(ofp, "\n");
}

//num to hex string, e.g. 0x73af
void tohexfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("tohex -- stack underflow! Needs a number on the stack\n");
        return;
    }
    DCLANG_INT val = (DCLANG_INT) dclang_pop();
    int bufsize = snprintf(NULL, 0, "0x%.2lx", val);
    char *str = dclang_malloc(bufsize + 1);
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

//num to string
void tostrfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("tostr -- needs a number on stack!\n");
        return;
    }
    DCLANG_FLT var = dclang_pop();
    int bufsize = snprintf(NULL, 0, "%g", var);
    char *str = dclang_malloc(bufsize + 1);
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

// string to number
void tonumfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("tonum -- needs a <str-pointer> on stack!\n");
        return;
    }
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("tonum -- String address out-of-range.\n");
        return;
    }
    char *mystr = (char *) string_PTR_addr;
    DCLANG_FLT num = strtod(mystr, NULL);
    push(num);
}

// character to number
void ordfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("ord -- stack underflow! Needs a single character in double-quotes on the stack.\n");
        return;
    }
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < (DCLANG_PTR) &memory_pool)
    {
        perror("ord -- String address out-of-range.\n");
        return;
    }
    char *string_loc = (char *) string_PTR_addr;
    push((int) *string_loc);
}

void tolowerfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("tolower -- needs a <source_str> pointer on the stack!\n");
        return;
    }
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("tolower -- String address out-of-range.\n");
        return;
    }
    char *mystr = (char *) string_PTR_addr;
    DCLANG_PTR buflen = (DCLANG_PTR) strlen(mystr);
    char *buf = (char *) dclang_malloc(buflen);
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
        printf("toupper -- needs a <source_str> pointer on the stack!\n");
        return;
    }
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("toupper -- String address out-of-range.\n");
        return;
    }
    char *mystr = (char *) string_PTR_addr;
    DCLANG_PTR buflen = (DCLANG_PTR) strlen(mystr);
    char *buf = (char *) dclang_malloc(buflen);
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

/////////////////////////////////////
// Standard libc string operations //
/////////////////////////////////////

void strlenfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("strlen -- stack underflow!\n");
        return;
    }
    DCLANG_PTR string_PTR_addr = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr < MIN_STR || string_PTR_addr > MAX_STR)
    {
        perror("strlen -- String address out-of-range.\n");
        return;
    }
    char *mystr = (char *) string_PTR_addr;
    push((DCLANG_PTR) strlen(mystr));
}

void streqfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("str= -- stack underflow!\n");
        return;
    }
    DCLANG_PTR string_PTR_addr2 = (DCLANG_PTR) dclang_pop();
    DCLANG_PTR string_PTR_addr1 = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr1 < MIN_STR || string_PTR_addr1 > MAX_STR)
    {
        perror("strlen -- First given string address out-of-range.\n");
        return;
    }
    if (string_PTR_addr2 < MIN_STR || string_PTR_addr2 > MAX_STR)
    {
        perror("strlen -- Second given string address out-of-range.\n");
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
        printf("str< -- stack underflow!\n");
        return;
    }
    DCLANG_PTR string_PTR_addr2 = (DCLANG_PTR) dclang_pop();
    DCLANG_PTR string_PTR_addr1 = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr1 < MIN_STR || string_PTR_addr1 > MAX_STR)
    {
        perror("str< -- First given string address out-of-range.\n");
        return;
    }
    if (string_PTR_addr2 < MIN_STR || string_PTR_addr2 > MAX_STR)
    {
        perror("str< -- Second given string address out-of-range\n");
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
        printf("str> -- stack underflow!\n");
        return;
    }
    DCLANG_PTR string_PTR_addr2 = (DCLANG_PTR) dclang_pop();
    DCLANG_PTR string_PTR_addr1 = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr1 < MIN_STR || string_PTR_addr1 > MAX_STR)
    {
        perror("str> -- First given string address out-of-range\n");
        return;
    }
    if (string_PTR_addr2 < MIN_STR || string_PTR_addr2 > MAX_STR)
    {
        perror("str> -- Second given string address out-of-range.\n");
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
        printf("strfind -- needs <haystack> <needle> string pointers on stack!\n");
        return;
    }
    DCLANG_PTR string_PTR_addr2 = (DCLANG_PTR) dclang_pop();
    DCLANG_PTR string_PTR_addr1 = (DCLANG_PTR) dclang_pop();
    if (string_PTR_addr1 < MIN_STR || string_PTR_addr1 > MAX_STR)
    {
        perror("strfind -- 'haystack' (first) string address out-of-range.\n");
        return;
    }
    if (string_PTR_addr2 < MIN_STR || string_PTR_addr2 > MAX_STR)
    {
        perror("strfind -- 'needle' (second) string address out-of-range.\n");
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
        perror("strspn -- <delim> string address out-of-range.\n");
        return;
    }
    if (str < MIN_STR || str > MAX_STR)
    {
        perror("strspn -- <str> string address out-of-range.\n");
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
        perror("strcspn -- <test_chars_str> string address out-of-range.\n");
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
        perror("strtok -- <str> (first) string address out-of-range.\n");
        return;
    }
    if (delim < MIN_STR || delim > MAX_STR)
    {
        perror("strtok -- <delim> (second) string address out-of-range.\n");
        return;
    }
    push((DCLANG_INT) strtok_r((char *)str, (char *)delim, savepoint_ptr));
}

void mempcpyfunc()
{
    if (data_stack_ptr < 3)
    {
        printf("mempcpy -- needs <dest> <source> <size> on stack!\n");
        return;
    }
    DCLANG_PTR size = (DCLANG_PTR) dclang_pop();
    DCLANG_PTR source = (DCLANG_PTR) dclang_pop();
    DCLANG_PTR dest = (DCLANG_PTR) dclang_pop();
    if ((dest != 0) && (dest < MIN_STR || dest > MAX_STR))
    {
        perror("mempcpy -- <dest> string address out-of-range.\n");
        return;
    }
    if (source < MIN_STR || source > MAX_STR)
    {
        perror("mempcpy -- <source> string address out-of-range.\n");
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
        printf("memset -- needs <dest_str> <char-int> <times-int> on stack!\n");
        return;
    }
    DCLANG_PTR times = (DCLANG_PTR) dclang_pop();
    DCLANG_PTR chr = (DCLANG_PTR) dclang_pop();
    DCLANG_PTR dest = (DCLANG_PTR) dclang_pop();
    if ((dest != 0) && (dest < MIN_STR || dest > MAX_STR))
    {
        perror("memset -- <dest> string address out-of-range.\n");
        return;
    }
    push((DCLANG_PTR)memset((char *)dest, (int)chr, (int)times));
}

////////////////////
// memory buffers //
////////////////////

void mkbuffunc()
{
    if (data_stack_ptr < 1)
    {
        printf("Stack_underflow!\n");
        printf("'mkbuf' needs <size-as-integer> on the stack\n");
    }
    DCLANG_PTR size = (DCLANG_PTR) dclang_pop();
    char *buf = (char *) dclang_malloc(size);
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
        printf("free -- stack underflow! N.B. This word is actually a no-op, kept for backwards compatibility\n");
        return;
    }
    DCLANG_PTR loc_PTR = (DCLANG_PTR) dclang_pop();
    dclang_free((char *) loc_PTR);
}

void memusedfunc()
{
    DCLANG_FLT memused = (DCLANG_FLT) (((float) unused_mem_idx) / ((float) MEMSIZE));
    push(memused);
}
