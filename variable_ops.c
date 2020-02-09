/* some simple variable functions, a bare-bones way of storing temp values...
Yes, bare-bones, but it works! */

MYFLT myvars[1048576];

static void pokefunc()
{
    if (data_stack_ptr < 2) {
        printf("stack underflow! ");
        return;
    }
    MYINT idx = (MYINT) pop();
    MYFLT val = pop();
    myvars[idx] = val;
}

static void peekfunc()
{
    if (data_stack_ptr < 1) {
        printf("stack underflow! ");
        return;
    }
    MYINT idx = (MYINT) pop();
    push(myvars[idx]);
}
