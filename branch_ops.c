typedef struct {
    long int limit;
    long int step;
} forloop_info;

forloop_info fl_stack[3];
int fl_ptr;

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
    --fl_ptr;
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

/* these 'for' loops are more flexible, allowing from/to/step parameters. */
static void forfunc()
{
    return_stack[return_stack_ptr++] = iptr;
    loop_counter[loop_counter_ptr++] = (long int) pop();
    fl_stack[fl_ptr].limit = (long int) pop();
    fl_stack[fl_ptr++].step = (long int) pop(); 
}

static void nextfunc()
{
    if (fl_stack[fl_ptr - 1].step > 0) {
        if (loop_counter[loop_counter_ptr - 1] < \
                (fl_stack[fl_ptr - 1].limit - fl_stack[fl_ptr - 1].step)) {
            loop_counter[loop_counter_ptr - 1] += fl_stack[fl_ptr - 1].step;
            iptr = return_stack[return_stack_ptr - 1];
        } else {
            loop_counter[--loop_counter_ptr] = 0;
            --return_stack_ptr;
            --fl_ptr;
        }
    } else {
        if (loop_counter[loop_counter_ptr - 1] > \
                (fl_stack[fl_ptr - 1].limit + fl_stack[fl_ptr - 1].step)) {
            loop_counter[loop_counter_ptr - 1] += fl_stack[fl_ptr - 1].step;
            iptr = return_stack[return_stack_ptr - 1];
        } else {
            loop_counter[--loop_counter_ptr] = 0;
            --return_stack_ptr;
            --fl_ptr;    
        }
    }
}

/* Crazy as this may seem, this function below is all that is needed to
replace if/then/else structures. */
static void skipfunc()
{
    int skipamt = (int) pop();
    iptr += skipamt;
}
