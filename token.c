/* all the logic for reading tokens from stdin goes here */

void add_to_buf(char ch) { if(bufused < IBUFSIZE - 1) buf[bufused++] = ch; }
char *buf2str()          { buf[bufused++] = '\0'; return strdup(buf); }

char *get_token() {
    MYINT ch;
    bufused = 0;
    /* skip leading spaces and comments */
    while (1) {
        /* skip leading space */
        do {
            if((ch = fgetc(stdin)) == EOF) exit(0);
        } while(isspace(ch));
        /* if we are starting a comment: */
        if (strchr("#", ch)) {
            /* go to the end of the line */
            do {
                if((ch = fgetc(stdin)) == EOF) exit(0);
            } while(! strchr("\n", ch));
        } else {
            add_to_buf(ch);
            break;
        }
    }
    /* grab all the next non-whitespace characters */
    while (1) {
        /* check again for EOF */
        if ((ch = fgetc(stdin)) == EOF) exit(0);
        if (isspace(ch)) {
            ungetc(ch, stdin);
            return buf2str();
        }
        add_to_buf(ch);
    }
}
