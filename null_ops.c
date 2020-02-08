static void nullfunc()
{
    void *ptr = NULL;
    push((MYINT)ptr);
}

static void isnullfunc()
{
    void *ptr = (void *)(MYINT)pop();
    push(ptr == NULL);
}
