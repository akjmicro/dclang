// struct for 'for' loops:
typedef struct {
    MYINT limit;
    MYINT step;
} forloop_info;

forloop_info fl_stack[3];
MYINT fl_ptr;

// array for 'do' loop max amounts:
MYINT do_info[3];
MYINT do_ptr;

// looping
static void dofunc()
{
    return_stack[return_stack_ptr++] = iptr;
    do_info[do_ptr++] = pop();
    loop_counter[loop_counter_ptr++] = 0;         
}

static void exitdofunc()
{
    loop_counter[--loop_counter_ptr] = 0;
    --return_stack_ptr;
}

static void redofunc()
{
    if (loop_counter[loop_counter_ptr - 1] < do_info[do_ptr - 1] - 1) {
        loop_counter[loop_counter_ptr - 1] += 1;
        iptr = return_stack[return_stack_ptr - 1];
    } else {
        loop_counter[--loop_counter_ptr] = 0;
        --return_stack_ptr;
        --do_ptr;
    }
}

/* these 'for' loops are more flexible, allowing from/to/step parameters. */
static void forfunc()
{
    return_stack[return_stack_ptr++] = iptr;
    fl_stack[fl_ptr].step = pop();
    loop_counter[loop_counter_ptr++] = pop();
    fl_stack[fl_ptr++].limit = pop();
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

/* Crazy as this may seem, this function below is all that is needed to
replace if/then/else structures. */
static void skipfunc()
{
    MYINT skipamt = pop();
    iptr += skipamt;
}
