// struct for 'for' loops:
typedef struct {
    DCLANG_LONG limit;
    DCLANG_LONG step;
} forloop_info;

forloop_info fl_stack[3];
DCLANG_LONG fl_ptr;

// array for 'times' loop max amounts:
DCLANG_LONG times_info[3];
DCLANG_LONG times_ptr;

// looping
void timesfunc()
{
    return_stack[return_stack_ptr++] = iptr;
    times_info[times_ptr++] = (DCLANG_LONG) dclang_pop();
    loop_counter[loop_counter_ptr++] = 0;
}

void _conttimes()
{
    loop_counter[loop_counter_ptr - 1] += 1;
    iptr = return_stack[return_stack_ptr - 1];
}

void exittimesfunc()
{
    loop_counter[--loop_counter_ptr] = 0;
    --return_stack_ptr;
    --times_ptr;
}

void againfunc()
{
    if (loop_counter[loop_counter_ptr - 1] < times_info[times_ptr - 1] - 1) {
        _conttimes();
    } else {
        exittimesfunc();
    }
}

/* these 'for' loops are more flexible, allowing from/to/step parameters. */
void forfunc()
{
    return_stack[return_stack_ptr++] = iptr;
    fl_stack[fl_ptr].step = (DCLANG_LONG) dclang_pop();
    loop_counter[loop_counter_ptr++] = (DCLANG_LONG) dclang_pop();
    fl_stack[fl_ptr++].limit = (DCLANG_LONG) dclang_pop();
}

void _contfor()
{
    loop_counter[loop_counter_ptr - 1] += fl_stack[fl_ptr - 1].step;
    iptr = return_stack[return_stack_ptr - 1];
}

void exitforfunc()
{
    --fl_ptr;
    loop_counter[--loop_counter_ptr] = 0;
    --return_stack_ptr;
}

void nextfunc()
{
    if (fl_stack[fl_ptr - 1].step > 0) {
        if (loop_counter[loop_counter_ptr - 1] < \
                (fl_stack[fl_ptr - 1].limit \
                 - fl_stack[fl_ptr - 1].step)) {
            _contfor();
        } else {
            exitforfunc();
        }
    } else {
        if (loop_counter[loop_counter_ptr - 1] > \
                (fl_stack[fl_ptr - 1].limit \
                 - fl_stack[fl_ptr - 1].step)) {
            _contfor();
        } else {
            exitforfunc();
        }
    }
}

void ifunc()
{
    push_no_check(loop_counter[loop_counter_ptr - 1]);
}

void jfunc()
{
    push_no_check(loop_counter[loop_counter_ptr - 2]);
}

void kfunc()
{
    push_no_check(loop_counter[loop_counter_ptr - 3]);
}


// jump if zero (false)
void jumpzfunc(DCLANG_FLT where)
{
    DCLANG_LONG truth = (DCLANG_LONG) dclang_pop();
    if (!truth) {
        iptr = (uintptr_t) where;
    }
}

// unconditional jump
void jumpufunc(DCLANG_FLT where)
{
    iptr = (uintptr_t) where;
}

// if-else-endif
void iffunc()
{
    // mark our location
    return_stack[return_stack_ptr++] = iptr;
    // set jump location for 'else'...w/o 'where' location
    // will be filled in by 'else'
    prog[iptr].function.with_param = jumpzfunc;
    prog[iptr++].param = 0;
}

void elsefunc()
{
    // get the last starting point of the 'if' clause
    DCLANG_LONG if_val = return_stack[--return_stack_ptr];
    // mark out current location on the return stack
    return_stack[return_stack_ptr++] = iptr;
    // set the unconditional jump, but no 'where' yet
    // (will be filled in later by 'endif')....
    prog[iptr].function.with_param = jumpufunc;
    prog[iptr++].param = 0;
    // update old if val goto:
    prog[if_val].param = iptr;
}


void endiffunc()
{
    DCLANG_LONG last_val = return_stack[--return_stack_ptr];
    prog[last_val].param = iptr;
}
