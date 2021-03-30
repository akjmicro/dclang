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
