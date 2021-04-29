sigset_t block_sigint;

static void blocksigintfunc()
{
  sigemptyset(&block_sigint);
  sigaddset(&block_sigint, SIGINT);
  sigprocmask(SIG_BLOCK, &block_sigint, NULL);
}


static void unblocksigintfunc()
{
  sigprocmask(SIG_UNBLOCK, &block_sigint, NULL);
}


static void forkfunc()
{
    // This function mainly exists so that a multi-client capable tcp/web server
    // can be had. It is assumed that the return value will be caught, inspected,
    // and handled by the caller, so this is really quite a simple c->dclang mapping.
    // TODO: a future enhancement might be to have a counting system in place for
    // avoiding fork-bomb DoS attacks. So, the introspection of a connection limit
    // before granting a new `fork`.
    push((DCLANG_INT32) fork());
}


static void exitfunc()
{
    if (data_stack_ptr < 1) {
        printf("exit -- need an integer exit code on the stack");
        return;
    }
    DCLANG_INT32 code = (DCLANG_INT32) pop();
    exit(code);
}
