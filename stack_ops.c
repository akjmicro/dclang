/* stack operations */
static void push(MYINT a)
{
    if (data_stack_ptr >= DATA_STACK_SIZE) {
        printf("push -- stack overflow!\n");
        data_stack_ptr = 0;
    }
    data_stack[data_stack_ptr++] = a;
}

static void push_no_check(MYINT a)
{
    data_stack[data_stack_ptr++] = a;
}

static MYINT pop()
{
    return data_stack[--data_stack_ptr];
}

static void dropfunc()
{
    if (data_stack_ptr < 1) {
        printf("drop -- stack underflow!\n");
        return;
    }
    --data_stack_ptr;
}

static void dupfunc()
{
    if (data_stack_ptr < 1) {
        printf("dup -- stack underflow!\n");
        return;
    }
    MYINT val = data_stack[data_stack_ptr - 1];
    push(val);
}

static void swapfunc()
{
    if (data_stack_ptr < 2) {
        printf("swap -- stack underflow!\n");
        return;
    }
    MYINT val1 = pop();
    MYINT val2 = pop();
    push_no_check(val1);
    push_no_check(val2); 
}

static void overfunc()
{
    if (data_stack_ptr < 2) {
        printf("over -- stack underflow!\n");
        return;
    }
    push(data_stack[data_stack_ptr - 2]);
}

static void rotfunc()
{
    if (data_stack_ptr < 3) {
        printf("rot -- stack underflow!\n");
        return;
    }
    /* a b c -- b c a */
    MYINT c = pop();
    MYINT b = pop();
    MYINT a = pop();
    push_no_check(b);
    push_no_check(c);
    push_no_check(a);
}

static void rotnegfunc()
{
    if (data_stack_ptr < 3) {
        printf("-rot -- stack underflow!\n");
        return;
    }
    /* a b c -- c a b */
    MYINT c = pop();
    MYINT b = pop();
    MYINT a = pop();
    push_no_check(c);
    push_no_check(a);
    push_no_check(b);
}

static void nipfunc()
{
    if (data_stack_ptr < 2) {
        printf("nip -- stack underflow!\n");
        return;
    }
    MYINT b = pop();
    pop();
    push_no_check(b);
}

static void tuckfunc()
{
    if (data_stack_ptr < 2) {
        printf("tuck -- stack underflow!\n");
        return;
    }
    MYINT val1 = pop();
    MYINT val2 = pop();
    push_no_check(val1);
    push_no_check(val2);
    push(data_stack[data_stack_ptr - 2]);
}

static void drop2func()
{
    if (data_stack_ptr < 2) {
        printf("2drop -- stack underflow!\n");
        return;
    }
    --data_stack_ptr;
    --data_stack_ptr;
}

static void dup2func()
{
    if (data_stack_ptr < 2) {
        printf("2dup -- stack underflow!\n");
        return;
    }
    MYINT val1 = data_stack[data_stack_ptr - 2];
    MYINT val2 = data_stack[data_stack_ptr - 1];
    push(val1);
    push(val2);
}

static void swap2func()
{
    if (data_stack_ptr < 4) {
        printf("2swap -- stack underflow!\n");
        return;
    }
    MYINT val1 = pop();
    MYINT val2 = pop();
    MYINT val3 = pop();
    MYINT val4 = pop();
    push_no_check(val2);
    push_no_check(val1);
    push_no_check(val4);
    push_no_check(val3); 
}

static void over2func()
{
    if (data_stack_ptr < 4) {
        printf("2over -- stack underflow!\n");
        return;
    }
    push(data_stack[data_stack_ptr - 4]);
    push(data_stack[data_stack_ptr - 4]);
}

static void rot2func()
{
    // ( 6 5 4 3 2 1 -- 4 3 2 1 6 5 )
    if (data_stack_ptr < 6) {
        printf("2rot -- stack underflow!\n");
        return;
    }
    MYINT val1 = pop();
    MYINT val2 = pop();
    MYINT val3 = pop();
    MYINT val4 = pop();
    MYINT val5 = pop();
    MYINT val6 = pop();
    push_no_check(val4);
    push_no_check(val3);
    push_no_check(val2);
    push_no_check(val1);
    push_no_check(val6);
    push_no_check(val5); 
}

static void rotneg2func()
{
    // ( 6 5 4 3 2 1 -- 2 1 6 5 4 3)
    if (data_stack_ptr < 6) {
        printf("-2rot -- Stack underflow!\n");
        return;
    }
    MYINT val1 = pop();
    MYINT val2 = pop();
    MYINT val3 = pop();
    MYINT val4 = pop();
    MYINT val5 = pop();
    MYINT val6 = pop();
    push_no_check(val2);
    push_no_check(val1);
    push_no_check(val6);
    push_no_check(val5);
    push_no_check(val4);
    push_no_check(val3); 
}

static void nip2func()
{
    // ( 4 3 2 1 -- 2 1 )
    if (data_stack_ptr < 4) {
        printf("2nip -- Stack underflow!\n");
        return;
    }
    MYINT val1 = pop();
    MYINT val2 = pop();
    pop();
    pop();
    push_no_check(val2);
    push_no_check(val1);
}

static void tuck2func()
{
    // ( 4 3 2 1 -- 2 1 4 3 2 1 )
    if (data_stack_ptr < 4) {
        printf("2tuck -- stack underflow!\n");
        return;
    }
    MYINT val1 = pop();
    MYINT val2 = pop();
    MYINT val3 = pop();
    MYINT val4 = pop();
    push_no_check(val2);
    push_no_check(val1);
    push_no_check(val4);
    push_no_check(val3);
    push(val2);
    push(val1);
}
