/* output */
static void printfunc()
{
    printf("%0.16Lg ", pop());
}

static void crfunc()
{
    printf("\n");
}

static void printstackfunc()
{
    int x;
    char *joiner;
    x = data_stack_ptr > 16 ? data_stack_ptr - 16 : 0;
    joiner = x == 0 ? " " : " ... "; 
    printf("<%i>%s", data_stack_ptr, joiner);
    for (x=0; x < data_stack_ptr; x++) {
        printf("%0.16Lg ", data_stack[x]);
    }
    printf("\n");
}
