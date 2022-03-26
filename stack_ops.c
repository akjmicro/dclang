/* stack operations */
void push(DCLANG_FLT a)
{
    if (data_stack_ptr >= DATA_STACK_SIZE) {
        printf("push -- stack overflow!\n");
        data_stack_ptr = 0;
    }
    data_stack[data_stack_ptr++] = a;
}

void push_no_check(DCLANG_FLT a)
{
    data_stack[data_stack_ptr++] = a;
}

DCLANG_FLT dclang_pop()
// special case -- has a return value b/c it can
// be used in other calls to give a value, which
// also means it can be an API call.
{
    return data_stack[--data_stack_ptr];
}

void dropfunc()
{
    if (data_stack_ptr < 1) {
        printf("drop -- stack underflow!\n");
        return;
    }
    --data_stack_ptr;
}

void dupfunc()
{
    if (data_stack_ptr < 1) {
        printf("dup -- stack underflow!\n");
        return;
    }
    push(data_stack[data_stack_ptr - 1]);
}

void overfunc()
{
    if (data_stack_ptr < 2) {
        printf("over -- stack underflow!\n");
        return;
    }
    push(data_stack[data_stack_ptr - 2]);
}

void pickfunc()
{
    if (data_stack_ptr < 1) {
         printf("pick -- stack underflow!\n");
         return;
    }
    DCLANG_UINT pick_idx = (DCLANG_UINT) dclang_pop();
    if (data_stack_ptr < (pick_idx + 1)) {
        printf("pick -- stack not deep enough!\n");
        return;
    }
    push(data_stack[data_stack_ptr - (pick_idx + 1)]);
}

void swapfunc()
{
    if (data_stack_ptr < 2) {
        printf("swap -- stack underflow!\n");
        return;
    }
    DCLANG_FLT val1 = dclang_pop();
    DCLANG_FLT val2 = dclang_pop();
    push_no_check(val1);
    push_no_check(val2);
}

void drop2func()
{
    if (data_stack_ptr < 2) {
        printf("2drop -- stack underflow!\n");
        return;
    }
    --data_stack_ptr;
    --data_stack_ptr;
}

void dup2func()
{
    if (data_stack_ptr < 2) {
        printf("2dup -- stack underflow!\n");
        return;
    }
    DCLANG_FLT val2 = data_stack[data_stack_ptr - 1];
    push(data_stack[data_stack_ptr - 2]);
    push(data_stack[data_stack_ptr - 2]);
}

void over2func()
{
    if (data_stack_ptr < 4) {
        printf("2over -- stack underflow!\n");
        return;
    }
    push(data_stack[data_stack_ptr - 4]);
    push(data_stack[data_stack_ptr - 4]);
}

void depthfunc()
{
    DCLANG_UINT size = data_stack_ptr;
    push((DCLANG_FLT)size);
}

void clearfunc()
{
    // clears the stack!
    data_stack_ptr = 0;
}

/////////////////////
// save data stack //
/////////////////////
void svpushfunc()
{
    if (save_data_stack_ptr >= DATA_STACK_SIZE) {
        printf("svpush -- stack overflow!\n");
        save_data_stack_ptr = 0;
    }
    DCLANG_FLT val = dclang_pop();
    save_data_stack[save_data_stack_ptr++] = val;
}

void svpopfunc()
{
    DCLANG_FLT val = save_data_stack[--save_data_stack_ptr];
    push_no_check(val);
}

void svdropfunc()
{
    if (save_data_stack_ptr < 1) {
        printf("svdrop -- stack underflow!\n");
        return;
    }
    --save_data_stack_ptr;
}

void svpickfunc()
{
    if (save_data_stack_ptr < 1) {
         printf("svpick -- stack underflow!\n");
         return;
    }
    DCLANG_UINT svpick_idx = (DCLANG_UINT) dclang_pop();
    if (save_data_stack_ptr < (svpick_idx + 1)) {
        printf("svpick -- stack not deep enough!\n");
        return;
    }
    push(save_data_stack[save_data_stack_ptr - (svpick_idx + 1)]);
}

void svdepthfunc()
{
    DCLANG_UINT size = save_data_stack_ptr;
    push((DCLANG_FLT)size);
}

void svclearfunc()
{
    // clears the stack!
    save_data_stack_ptr = 0;
}
