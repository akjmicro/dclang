void nullfunc()
{
    void *ptr = NULL;
    push((DCLANG_INT)ptr);
}

void isnullfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'isnull' needs an element on the stack!\n");
        return;
    }
    void *ptr = (void *)(DCLANG_INT)dclang_pop();
    push(ptr == NULL);
}
