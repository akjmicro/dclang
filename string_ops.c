/* unused as of yet */

char string_pad[1048576];
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
            /* backspace */
            if (strchr("b", ch)) {
                ch = 8;
            }
        }
        string_pad[string_here++] = ch;
        if ((ch = fgetc(stdin)) == EOF) exit(0);
    }
    long double string_addy = \
        (long double)((unsigned long int)&string_pad + string_start);
    long double string_size = (long double)(string_here - string_start);
    if (def_mode) {
        prog[iptr].function.with_param = push;
        prog[iptr++].param = string_addy;
        prog[iptr].function.with_param = push;
        prog[iptr++].param = string_size; 
    } else {
        push(string_addy);
        push(string_size);
    }
}

static void printfunc()
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

static void emitfunc()
{
    unsigned int char_code = (int) pop();
    printf("%lc", char_code);
}
