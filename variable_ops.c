/* some simple variable functions, a bare-bones way of storing temp values...
Yes, bare-bones, but it works! */

MYFLT myvars[1048576];
MYUINT varsidx;

static void pokefunc()
{
    if (data_stack_ptr < 2) {
        printf("! -- stack underflow! ");
        return;
    }
    MYINT idx = (MYINT) pop();
    MYFLT val = pop();
    myvars[idx] = val;
}

static void peekfunc()
{
    if (data_stack_ptr < 1) {
        printf("@ -- stack underflow! ");
        return;
    }
    MYINT idx = (MYINT) pop();
    push(myvars[idx]);
}

/* implement constants */
static void constantfunc()
{
    startword();
    prog[iptr].function.with_param = push;
    prog[iptr++].param = pop();
    endword();
}

static void _variable_common()
{
    // In reality, a variable is a named word that simply pushes
    // an address to its associated memory
    prog[iptr].function.with_param = push;
    prog[iptr++].param = varsidx++;
}

static void variablefunc()
{
    if (iptr < max_iptr) {
        // mark current position
        MYUINT ret_iptr = iptr;
        // jump to end (when this is called in the future)
        iptr = return_stack[--return_stack_ptr];
        return_stack[return_stack_ptr++] = ret_iptr;
        startword();
        _variable_common();
        // part of endword here:
        prog[iptr++].function.without_param = returnfunc;
        max_iptr = iptr;
        // restore function position
        iptr = return_stack[--return_stack_ptr];
        // restore return stack to new hishest position
        return_stack[return_stack_ptr++] = max_iptr;
    } else {
        startword();
        _variable_common();
        endword();
    }
}

static void allotfunc()
{
    if (data_stack_ptr < 1) {
        printf("allot -- stack underflow! ");
        return;
    }
    varsidx += (MYUINT) pop() - 1;
}

static void createfunc()
{
    variablefunc();
    --varsidx;
}

static void commafunc()
{
    if (data_stack_ptr < 1) {
        printf(", -- stack underflow! ");
        return;
    }
    MYFLT val = pop();
    myvars[varsidx++] = val;
}

/*

00 - (def1)
01 -
02 -
03 return
04 - (def2)
05 def1
06 -
07 return
08 - (def3)
09 -
10 var
11 -
12 return
13 new-var-func (was last HERE (now "varname"))
14 return
15 HERE

00 - (def1)
01 -
02 -
03 return
04 - (def2)
05 def1
06 -
07 return
08 - (def3)
09 -
10 create
11 -
12 return
13 new-create-func (was last HERE (now "varname"))
14 return
15 new-var-name-push (var)
16 return

return stack (top is rightmost): 15

*/
