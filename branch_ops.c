/* looping */
static void dofunc()
{
    return_stack[return_stack_ptr++] = iptr;
    loop_counter_ptr++;
}

static void exitfunc()
{
    loop_counter[--loop_counter_ptr] = 0;
    --return_stack_ptr;
}

static void redofunc()
{
    if (pop() != 0) {
        loop_counter[loop_counter_ptr - 1] += 1;
        iptr = return_stack[return_stack_ptr - 1];
    } else {
        exitfunc();
    }
}

static void ifunc()
{
    push(loop_counter[0]);
}

static void jfunc()
{
    push(loop_counter[1]);  
}

/* Crazy as this may seem, this function below is all that is needed to
replace if/then/else structures. */
static void skipfunc()
{
    int skipamt = (int) pop();
    iptr += skipamt;
}

/* This function will restore 'iptr' to what it was before going on its
fancy journey into a function.  It won't "Make America Great Again", but it's
a start.  */
static void returnfunc()
{
    if (return_stack_ptr == 0) {
        iptr = num_insts;
    } else {
        iptr = return_stack[--return_stack_ptr];
    }
}
