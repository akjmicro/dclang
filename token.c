void revertinput(void);

// utf-8 char buffer
char utf8_buf[5];

long utf8_encode(char *out, uint64_t utf) {
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

int get_unicode_by_hex(char *chbuf, int usize, char **line_ptr) {
    char numstr[usize];
    for (int i = 0; i < usize - 1; i++) {
        if (**line_ptr == '\0') return 0; // End of line reached unexpectedly
        numstr[i] = **line_ptr;
        (*line_ptr)++;
    }
    numstr[usize - 1] = '\0'; // Null terminate
    int ucode = strtol(numstr, NULL, 16);
    int num_bytes_ret = utf8_encode(chbuf, ucode);
    return num_bytes_ret > 0 ? 1 : 0;
}

int get_ascii(char *chbuf, int usize, char **line_ptr) {
    char numstr[usize];
    for (int i = 0; i < usize - 1; i++) {
        if (**line_ptr == '\0') return 0; // End of line reached unexpectedly
        numstr[i] = **line_ptr;
        (*line_ptr)++;
    }
    numstr[usize - 1] = '\0'; // Null terminate
    int acode = strtol(numstr, NULL, 16);
    chbuf[0] = (char) acode;
    chbuf[1] = 0;
    return 1;
}

char get_char() {
    static char *rocket_prompt = "🚀dclang=> ";
    static char *continue_prompt = "🔗...=> ";
    // If we're at the end of the buffer, read a new line
    if (*line_ptr == '\0') {
        if (live_repl) {
            // Show the prompt in interactive mode
            fprintf(ofp, "%s", (in_string || def_mode) ? continue_prompt : rocket_prompt);
            fflush(ofp);
        }
        if (!fgets(line_buf, 256, ifp)) {
            return EOF;  // End of input (CTRL+D or file EOF)
        }
        line_ptr = line_buf;
    }
    return (*line_ptr) ? *line_ptr++ : EOF;
}

void stringfunc() {
    char ch, escape_ch, chbuf[5];
    int stat = -1;
    char *scratch = &memory_pool[unused_mem_idx];
    char *scratch_start = scratch;
    in_string = 1;
    // Get the first character
    if ((ch = get_char()) == EOF) exit(0);
    while (ch != '"') {
        if (ch == '\\') {
            // Handle escape sequences
            if ((escape_ch = get_char()) == EOF) exit(0);
            switch (escape_ch) {
                case 'b': chbuf[0] = 8; break;   // Backspace
                case 't': chbuf[0] = 9; break;   // Tab
                case 'n': chbuf[0] = 10; break;  // Newline
                case 'r': chbuf[0] = 13; break;  // Carriage return
                case 'x': stat = get_ascii(chbuf, 3, &line_ptr); goto check_valid;
                case 'u': stat = get_unicode_by_hex(chbuf, 5, &line_ptr); goto check_valid;
                case 'U': stat = get_unicode_by_hex(chbuf, 9, &line_ptr); goto check_valid;
                default:  chbuf[0] = escape_ch; break; // Literal char
            }
            chbuf[1] = 0;
        } else {
            // Regular character
            chbuf[0] = ch;
            chbuf[1] = 0;
        }
    check_valid:
        if (stat == 0) {
            printf("Illegal escape sequence in string.\n");
            return;
        }
        scratch = mempcpy(scratch, chbuf, strlen(chbuf));
        if ((ch = get_char()) == EOF) exit(0);
        continue;
    }
    *scratch = '\0'; // Null-terminate string
    int chr_cnt = (scratch - scratch_start) + 1;
    unused_mem_idx = (unused_mem_idx + chr_cnt + 0x0f) & ~0x0f;
    // Register string memory range
    DCLANG_PTR string_dest_ptr = (DCLANG_PTR) scratch_start;
    DCLANG_PTR buflen = (DCLANG_PTR) chr_cnt;
    MIN_STR = (MIN_STR == 0 || string_dest_ptr < MIN_STR) ? string_dest_ptr : MIN_STR;
    MAX_STR = (MAX_STR == 0 || string_dest_ptr + buflen > MAX_STR) ? string_dest_ptr + buflen : MAX_STR;
    // Handle stack or program storage
    if (def_mode) {
        prog[iptr].opcode = OP_PUSH;
        prog[iptr++].param = string_dest_ptr;
    } else {
        push(string_dest_ptr);
    }
    in_string = 0;
}

// Helpers for `get_token()`

void add_to_buf(char ch) {
    if((bufused < IBUFSIZE - 1) && ch != EOF) {
        buf[bufused++] = ch;
    }
}

char *buf2str() {
    buf[bufused++] = '\0';
    return dclang_strdup(buf);
}

// End helpers for `get_token()`

char *get_token() {
    DCLANG_LONG ch;
    bufused = 0;
    // Skip leading spaces and handle comments
    while ((ch = get_char()) != EOF) {
        if (isspace(ch)) continue;
        switch (ch) {
            case '#': // Comment detected, skip to end of line
                while ((ch = get_char()) != EOF && ch != '\n');
                continue;
            case '"': // String detected, handle it separately
                stringfunc();
                continue;
            default:
                add_to_buf(ch);
                goto read_token;
        }
    }
    // Handle EOF case
    revertinput();
    return "EOF";
  read_token:
    // Read remaining characters until whitespace or EOF
    while ((ch = get_char()) != EOF) {
        if (isspace(ch)) {
            //ungetc(ch, ifp);  // Put back the whitespace
            return buf2str();
        }
        add_to_buf(ch);
    }
    // Handle EOF case at the end
    revertinput();
    return "EOF";
}
