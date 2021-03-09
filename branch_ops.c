// struct for 'for' loops:
typedef struct {
    DCLANG_INT limit;
    DCLANG_INT step;
} forloop_info;

forloop_info fl_stack[3];
DCLANG_INT fl_ptr;

// array for 'times' loop max amounts:
DCLANG_INT times_info[3];
DCLANG_INT times_ptr;

// looping
static void timesfunc()
{
    return_stack[return_stack_ptr++] = iptr;
    times_info[times_ptr++] = (DCLANG_INT) pop();
    loop_counter[loop_counter_ptr++] = 0;
}

static void _conttimes()
{
    loop_counter[loop_counter_ptr - 1] += 1;
    iptr = return_stack[return_stack_ptr - 1];
}

static void exittimesfunc()
{
    loop_counter[--loop_counter_ptr] = 0;
    --return_stack_ptr;
    --times_ptr;
}

static void againfunc()
{
    if (loop_counter[loop_counter_ptr - 1] < times_info[times_ptr - 1] - 1) {
        _conttimes();
    } else {
        exittimesfunc();
    }
}

/* these 'for' loops are more flexible, allowing from/to/step parameters. */
static void forfunc()
{
    return_stack[return_stack_ptr++] = iptr;
    fl_stack[fl_ptr].step = (DCLANG_INT) pop();
    loop_counter[loop_counter_ptr++] = (DCLANG_INT) pop();
    fl_stack[fl_ptr++].limit = (DCLANG_INT) pop();
}

static void _contfor()
{
    loop_counter[loop_counter_ptr - 1] += fl_stack[fl_ptr - 1].step;
    iptr = return_stack[return_stack_ptr - 1];
}

static void exitforfunc()
{
    --fl_ptr;
    loop_counter[--loop_counter_ptr] = 0;
    --return_stack_ptr;
}

static void nextfunc()
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

static void ifunc()
{
    push_no_check(loop_counter[loop_counter_ptr - 1]);
}

static void jfunc()
{
    push_no_check(loop_counter[loop_counter_ptr - 2]);
}

static void kfunc()
{
    push_no_check(loop_counter[loop_counter_ptr - 3]);
}


// jump if zero (false)
static void jumpzfunc()
{
    DCLANG_INT where = (DCLANG_INT) pop();
    DCLANG_INT truth = (DCLANG_INT) pop();
    if (!truth) {
        iptr = where;
    }
}

// unconditional jump
static void jumpufunc()
{
    DCLANG_INT where = (DCLANG_INT) pop();
    iptr = where;
}

// if-else-endif
static void iffunc()
{
    // mark our location
    return_stack[return_stack_ptr++] = iptr;
    prog[iptr].function.with_param = push;
    prog[iptr++].param = 0;
    prog[iptr++].function.without_param = jumpzfunc;
}

static void elsefunc()
{
    // get the last 'if' value
    DCLANG_INT if_val = return_stack[--return_stack_ptr];
    // replace the return stack location value with 'here'
    // so, we are re-marking our location
    return_stack[return_stack_ptr++] = iptr;
    // push the next unconditional jump value, starting with a dummy
    prog[iptr].function.with_param = push;
    prog[iptr++].param = 0;
    prog[iptr++].function.without_param = jumpufunc;
    // update old if val goto:
    prog[if_val].param = iptr - 1;
}


static void endiffunc()
{
    DCLANG_INT last_val = return_stack[--return_stack_ptr];
    prog[last_val].param = iptr - 1;
}
