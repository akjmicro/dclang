/* output */
static void showfunc()
{
    printf("%0.16g ", pop());
}

static void crfunc()
{
    printf("\n");
}

static void showstackfunc()
{
    MYINT x;
    char *joiner;
    x = data_stack_ptr > 16 ? data_stack_ptr - 16 : 0;
    joiner = x == 0 ? " " : " ... "; 
    printf("<%li>%s", data_stack_ptr, joiner);
    for (x=0; x < data_stack_ptr; x++) {
        printf("%0.16g ", data_stack[x]);
    }
    printf("\n");
}

static void showrjfunc()
{
// right-justified for pretty printing!
    int precision = (int) pop();
    int width = (int) pop();
    printf("% *.*g ", width, precision, pop());
}
