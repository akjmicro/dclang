// utf-8 char buffer
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


void add_to_buf(char ch) {
    if((bufused < IBUFSIZE - 1) && ch != EOF) {
        buf[bufused++] = ch;
    }
}

char *buf2str() {
    buf[bufused++] = '\0';
    return dclang_strdup(buf);
}

void setinput(FILE *fp)  {
    file_stack[fsp++] = ifp;
    ifp = fp;
}

void revertinput() {
    if (fsp == 0) {
        exit(0);
    }
    ifp = file_stack[--fsp];
}

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
            // consume an extra char due to backslash
            if ((escape_ch = fgetc(ifp)) == EOF) exit(0);
            switch(escape_ch)
            {
                // backspace
                case 'b' :
                  chbuf[0] = 8;
                  chbuf[1] = 0;
                  break;
                // tab
                case 't' :
                    chbuf[0] = 9;
                    chbuf[1] = 0;
                    break;
                // newline
                // (line-feed and carriage return together on unix)
                case 'n' :
                    chbuf[0] = 10;
                    chbuf[1] = 0;
                    break;
                // carriage return
                case 'r' :
                    chbuf[0] = 13;
                    chbuf[1] = 0;
                    break;
                // 1-byte ASCII code
                case 'x' :
                    stat = get_ascii(chbuf, 3);
                    if (stat == 0)
                    {
                        printf("Illegal 1-byte ASCII string denoted with \\x.\n");
                        return;
                    }
                    break;
                // 2-byte unicode
                case 'u' :
                    stat = get_unicode_by_hex(chbuf, 5);
                    if (stat == 0)
                    {
                        printf("Illegal 2-byte unicode entry in string.\n");
                        return;
                    }
                    break;
                // 4-byte unicode
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
        prog[iptr].opcode = OP_PUSH;
        prog[iptr++].param = string_dest_ptr;
    } else {
        push(string_dest_ptr);
    }
}

char *get_token() {
    DCLANG_LONG ch;
    bufused = 0;
    // skip leading spaces and comments
    while (1) {
        // skip leading space
        do {
            if((ch = fgetc(ifp)) == EOF) {
                revertinput();
                return "EOF";
            }
        } while(isspace(ch));
        // if we are starting a comment:
        if (strchr("#", ch)) {
            // go to the end of the line
            do {
                if((ch = fgetc(ifp)) == EOF) {
                    revertinput();
                    return "EOF";
                }
            } while(! strchr("\n", ch));
            continue;
        }
        // is this a string?
        if (strchr("\"", ch)) {
            // call the sub-routine to deal with the string:
            stringfunc();
            continue;
        } else {
            add_to_buf(ch);
            break;
        }
    }
    // grab all the next non-whitespace characters
    while (1) {
        // check again for EOF
        if ((ch = fgetc(ifp)) == EOF) {
            revertinput();
            return "EOF";
        }
        if (isspace(ch)) {
            ungetc(ch, ifp);
            return buf2str();
        }
        add_to_buf(ch);
    }
}
