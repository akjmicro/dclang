// struct for 'for' loops:
typedef struct {
    MYINT limit;
    MYINT step;
} forloop_info;

forloop_info fl_stack[3];
MYINT fl_ptr;

// array for 'do' loop max amounts:
MYINT times_info[3];
MYINT times_ptr;

// looping
static void timesfunc()
{
    return_stack[return_stack_ptr++] = iptr;
    times_info[times_ptr++] = (MYINT) pop();
    loop_counter[loop_counter_ptr++] = 0;         
}

static void exittimesfunc()
{
    loop_counter[--loop_counter_ptr] = 0;
    --return_stack_ptr;
}

static void againfunc()
{
    if (loop_counter[loop_counter_ptr - 1] < times_info[times_ptr - 1] - 1) {
        loop_counter[loop_counter_ptr - 1] += 1;
        iptr = return_stack[return_stack_ptr - 1];
    } else {
        loop_counter[--loop_counter_ptr] = 0;
        --return_stack_ptr;
        --times_ptr;
    }    
}

/* these 'for' loops are more flexible, allowing from/to/step parameters. */
static void forfunc()
{
    return_stack[return_stack_ptr++] = iptr;
    fl_stack[fl_ptr].step = (MYINT) pop();
    loop_counter[loop_counter_ptr++] = (MYINT) pop();
    fl_stack[fl_ptr++].limit = (MYINT) pop(); 
}

static void exitforfunc()
{
    fl_ptr = --fl_ptr < 1 ? 1 : fl_ptr;
    loop_counter[--loop_counter_ptr] = fl_stack[fl_ptr - 1].limit;
    --return_stack_ptr;
}

static void nextfunc()
{
    if (fl_stack[fl_ptr - 1].step > 0) {
        if (loop_counter[loop_counter_ptr - 1] < \
                (fl_stack[fl_ptr - 1].limit \
                 - fl_stack[fl_ptr - 1].step)) {
            loop_counter[loop_counter_ptr - 1] += fl_stack[fl_ptr - 1].step;
            iptr = return_stack[return_stack_ptr - 1];
        } else {
            exitforfunc();
        }
    } else {
        if (loop_counter[loop_counter_ptr - 1] > \
                ((fl_stack[fl_ptr - 1].limit) \
                  - fl_stack[fl_ptr - 1].step)) {
            loop_counter[loop_counter_ptr - 1] += fl_stack[fl_ptr - 1].step;
            iptr = return_stack[return_stack_ptr - 1];
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
    MYINT where = (MYINT) pop();
    MYINT truth = (MYINT) pop();
    if (!truth) {
        iptr = where;
    }
}

// unconditional jump
static void jumpufunc()
{
    MYINT where = (MYINT) pop();
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
    MYINT if_val = return_stack[--return_stack_ptr];
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
    MYINT last_val = return_stack[--return_stack_ptr];
    prog[last_val].param = iptr - 1;
}
