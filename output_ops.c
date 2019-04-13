/* output */
static void showfunc()
{
    if (data_stack_ptr < 1) {
        printf("stack underflow! ");
        return;
    }
    printf("%li ", pop());
}

static void showunsignedfunc()
{
    if (data_stack_ptr < 1) {
        printf("stack underflow! ");
        return;
    }
    printf("%lu ", pop());
}

static void showhexfunc()
{
    if (data_stack_ptr < 1) {
        printf("stack underflow! ");
        return;
    }
    printf("0x%x ", pop());
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
        printf("%li ", data_stack[x]);
    }
    printf("\n");
}

static void showunsignedstackfunc()
{
    MYINT x;
    char *joiner;
    x = data_stack_ptr > 16 ? data_stack_ptr - 16 : 0;
    joiner = x == 0 ? " " : " ... ";
    printf("<%lu>%s", data_stack_ptr, joiner);
    for (x=0; x < data_stack_ptr; x++) {
        printf("%lu ", data_stack[x]);
    }
    printf("\n");
}

static void showhexstackfunc()
{
    MYINT x;
    char *joiner;
    x = data_stack_ptr > 16 ? data_stack_ptr - 16 : 0;
    joiner = x == 0 ? " " : " ... ";
    printf("<0x%x>%s", data_stack_ptr, joiner);
    for (x=0; x < data_stack_ptr; x++) {
        printf("0x%x ", data_stack[x]);
    }
    printf("\n");
}

static void showrjfunc()
{
    if (data_stack_ptr < 3) {
        printf("Stack underflow! '.rj' needs: value, width, precision on the stack\n");
        return;
    }
    // right-justified for pretty printing!
    int precision = (MYINT) pop();
    int width = (MYINT) pop();
    printf("%*.*g ", width, precision, pop());
}
