/* Here we add the functionality necessary for the user to define named
 procedures (words). The idea is to have a struct that contains:
 1) a string naming the word.
 2) the position of that word in the 'prog' array.

 The difference between this and the already defined primitives is that the
 2nd element in the primitives struct was a function pointer.  Here, it is
 an index (which is also a pointer) to the start of a procedure, which
 itself may contain other procedures that the code may jump to, or simply
 resolve to some primitives.

 In the 'prog' array, which is indexed by 'iptr', in effect, a word call
 is simply a jump to the start of that procedure.  When the 'compile'
 function finds that a user-defined bit of code is being referenced, what is
 put into 'prog' is a call to 'callfunc', with the parameter to that call
 being the index in 'prog' where the word resides. */


/* for debugging */
void showdefined()
{
    for (int x=0; x < num_user_words; x++) {
        printf("Word %i: %s @ %li\n", x, user_words[x].name,\
                                         user_words[x].word_start);
    }
}

DCLANG_LONG dclang_findword(const char *word)
{
    for (DCLANG_LONG x = num_user_words - 1; x > -1 ; x--) {
        if (strcmp(user_words[x].name, word) == 0) {
            return user_words[x].word_start;
        }
    }
    return -1;
}

void callword(DCLANG_FLT where)
{
    locals_base_idx += 8;
    // mark where we are for restoration later
    return_stack[return_stack_ptr++] = iptr;
    // set word target; execute word target
    iptr = (DCLANG_PTR) where;
    (*(prog[iptr].function.with_param)) (prog[iptr++].param);
    // adjust the locals base pointer
}

void dclang_callword(DCLANG_PTR where)
{
    callword((DCLANG_FLT) where);
    // execute all until we reach the end of the iptr queue
    while (iptr < max_iptr) {
        (*(prog[iptr].function.with_param)) (prog[iptr++].param);
    }
}

void returnfunc()
{
    // restore locals_base_idx
    locals_base_idx -= 8;
    // restore the old iptr
    iptr = return_stack[--return_stack_ptr];
    // adjust the locals pointers
}

/* respond to ':' token: */
void startword()
{
    /* grab name */
    char *this_token;
    /* TODO: validation */
    this_token = get_token();
    /* put name and current location in user_words lookup array */
    user_words[num_user_words].name = this_token;
    user_words[num_user_words++].word_start = iptr;
}

/* respond to ';' token: */
void endword()
{
    /* Simply insert a return call into 'prog' where 'iptr' now points. */
    prog[iptr++].function.without_param = returnfunc;
    max_iptr = iptr;
}

// deal with setting up local variables

/*
void startlocals()
{
    locals_idx = 0;
    char *new_token = get_token();
    while (strcmp(new_token, "}") != 0) {
        locals_names[locals_idx] = new_token;
        locals_idx += 1;
    }
}

void endlocals()
{

}
*/
