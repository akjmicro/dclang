/* unused as of yet */

char string_pad[4096];
int string_here;

static void stringfunc()
{
    int ch;
    /* get a starting marker for length */
    int string_start = string_here;
    
    if ((ch = fgetc(stdin)) == EOF) exit(0);
    while (! strchr("\"", ch)) {
        if (strchr("\\", ch)) {
            /* consume an extra char due to backslash */            
            if ((ch = fgetc(stdin)) == EOF) exit(0);
        }
        string_pad[string_here++] = ch;
        if ((ch = fgetc(stdin)) == EOF) exit(0);
    }
    push((long double)((unsigned long int)&string_pad + string_start));
    push(string_here - string_start);
}

static void typefunc()
{
    if (data_stack_ptr < 2) {
        printf("stack underflow! ");
        return;
    }
    unsigned int str_len = (unsigned int) pop();
    char *str_start = (char *)((unsigned long int) pop());
    char dest[str_len];
    memcpy(dest, (char *)(str_start + 1), str_len);
    printf("%s", dest);        
}

static void commentfunc()
{
    int ch;
    /* do nothing until a newline */    
    if ((ch = fgetc(stdin)) == EOF) exit(0);
    while (! strchr("\n", ch)) {
        if ((ch = fgetc(stdin)) == EOF) exit(0);
    }
}
