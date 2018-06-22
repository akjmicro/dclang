/* some simple variable functions, a bare-bones way of storing temp values... 
Yes, bare-bones, but it works! */

long double myvars[4096];

static void pokefunc()
{
    if (data_stack_ptr < 2) {
        printf("stack underflow! ");
        return;
    }
    int idx = (int) pop();
    long double val = pop();
    myvars[idx] = val;
}

static void peekfunc()
{
    if (data_stack_ptr < 1) {
        printf("stack underflow! ");
        return;
    }
    int idx = (int) pop();
    push(myvars[idx]);
}
