static void nullfunc()
{
    void *ptr = NULL;
    push((MYINT)ptr);
}

static void isnullfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'isnull' needs an element on the stack!\n");
        return;
    }
    void *ptr = (void *)(MYINT)pop();
    push(ptr == NULL);
}
