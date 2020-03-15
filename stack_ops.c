/* stack operations */
static void push(MYFLT a)
{
    if (data_stack_ptr >= DATA_STACK_SIZE) {
        printf("push -- stack overflow!\n");
        data_stack_ptr = 0;
    }
    data_stack[data_stack_ptr++] = a;
}

static void push_no_check(MYFLT a)
{
    data_stack[data_stack_ptr++] = a;
}

static MYFLT pop()
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
    push(data_stack[data_stack_ptr - 1]);
}

static void swapfunc()
{
    if (data_stack_ptr < 2) {
        printf("swap -- stack underflow!\n");
        return;
    }
    MYFLT val1 = pop();
    MYFLT val2 = pop();
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
    MYFLT c = pop();
    MYFLT b = pop();
    MYFLT a = pop();
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
    MYFLT c = pop();
    MYFLT b = pop();
    MYFLT a = pop();
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
    MYFLT b = pop();
    pop();
    push_no_check(b);
}

static void tuckfunc()
{
    if (data_stack_ptr < 2) {
        printf("tuck -- stack underflow!\n");
        return;
    }
    MYFLT val1 = pop();
    MYFLT val2 = pop();
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
    MYFLT val2 = data_stack[data_stack_ptr - 1];
    push(data_stack[data_stack_ptr - 2]);
    push(data_stack[data_stack_ptr - 2]);
}

static void swap2func()
{
    if (data_stack_ptr < 4) {
        printf("2swap -- stack underflow!\n");
        return;
    }
    MYFLT val1 = pop();
    MYFLT val2 = pop();
    MYFLT val3 = pop();
    MYFLT val4 = pop();
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
    MYFLT val1 = pop();
    MYFLT val2 = pop();
    MYFLT val3 = pop();
    MYFLT val4 = pop();
    MYFLT val5 = pop();
    MYFLT val6 = pop();
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
    MYFLT val1 = pop();
    MYFLT val2 = pop();
    MYFLT val3 = pop();
    MYFLT val4 = pop();
    MYFLT val5 = pop();
    MYFLT val6 = pop();
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
    MYFLT val1 = pop();
    MYFLT val2 = pop();
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
    MYFLT val1 = pop();
    MYFLT val2 = pop();
    MYFLT val3 = pop();
    MYFLT val4 = pop();
    push_no_check(val2);
    push_no_check(val1);
    push_no_check(val4);
    push_no_check(val3);
    push(val2);
    push(val1);
}

static void pickfunc()
{
    if (data_stack_ptr < 1) {
         printf("pick -- stack underflow!\n");
         return;
    }
    MYUINT pick_idx = (MYUINT) pop();
    if (data_stack_ptr < (pick_idx + 1)) {
        printf("pick -- stack not deep enough!\n");
        return;
    }
    push(data_stack[data_stack_ptr - (pick_idx + 1)]);
}

static void clearfunc()
{
    // clears the stack!
    data_stack_ptr = 0;
}

/////////////////////
// save data stack //
/////////////////////
static void savepush()
{
    if (save_data_stack_ptr >= DATA_STACK_SIZE) {
        printf("savepush -- stack overflow!\n");
        save_data_stack_ptr = 0;
    }
    MYFLT val = pop();
    save_data_stack[save_data_stack_ptr++] = val;
}

static void savepop()
{
    MYFLT val = save_data_stack[--save_data_stack_ptr];
    push_no_check(val);
}
