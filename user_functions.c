/* Here we add the functionality necessary for the user to define named
 procedures (functions). The idea is to have a struct that contains:
 1) a string naming the function
 2) the position of that function in the 'prog' array.
 
 The difference between this and the already defined primitives is that the
 2nd element in the primitives struct was a function pointer.  Here, it is
 an index (which is also a pointer) to the start of a procedure, which
 itself may contain other procedures that the code may jump to, or simply
 resolve to some primitives. 
 
 In the 'prog' array, which is indexed by 'iptr', in effect, a function call
 is simply a jump to the start of that procedure.  When the 'compile'
 function finds that a user-defined bit of code is being referenced, what is
 put into 'prog' is a call to 'goto_func', with the parameter to that call
 being the index in 'prog' where the function resides. */

typedef struct {
    const char *name;
    long int func_start;
} user_function;

user_function user_functions[1024];
int num_user_functions;

/* debugging */
static void showdefined()
{
    for (int x=0; x < num_user_functions; x++) {
        printf("Function %i: %s @ %li\n", x, user_functions[x].name,\
                                            user_functions[x].func_start);
    }
}

static void gotofunc(long double where)
{
    return_stack[return_stack_ptr++] = iptr;
    iptr = (long int) where;
    (*(prog[iptr].function.with_param)) (prog[iptr].param);
}

/* This function will restore 'iptr' to what it was before going on its
fancy journey into a function.  It won't "Make America Great Again", but it's
a start.  */
static void returnfunc()
{
    iptr = return_stack[--return_stack_ptr];
}

/* respond to '[' token: */
static void startdeffunc()
{
    /* grab name */
    char *this_token;
    /* TODO: validation */
    this_token = get_token();
    /* put name and current location in user_functions lookup array */
    user_functions[num_user_functions].name = this_token;
    user_functions[num_user_functions].func_start = iptr;
}

/* respond to ']' token: */
static void enddeffunc()
{
    /* Simply insert a return call into 'prog' where 'iptr' now points. */
    prog[iptr++].function.without_param = returnfunc;
    /* we added a function, so increment how many we know exist */
    ++num_user_functions;
    // for debugging:
    //showdefined();
}

/* 
     (foo)   push(2);  0
             *;        1
             return;   2
     (bar)   push(3);  3
             *;        4
             return;   5
   (fubar)   push(1);  6
             -;        7
             goto(foo); 8     
             goto(bar); 9
             return;    10
             --         11

*/
