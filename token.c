// All the logic for reading tokens from stdin and file pointers goes here

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
