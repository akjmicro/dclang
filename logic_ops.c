/* logical words */
static void andfunc()
{
  push((MYINT) pop() & (MYINT) pop());
}

static void orfunc()
{
  push((MYINT) pop() | (MYINT) pop());
}

static void xorfunc()
{
  push((MYINT) pop() ^ (MYINT) pop());
}

static void notfunc()
{
  push(~(MYINT) pop());
}

/* comparison booleans */
static void eqfunc()
{
  push((MYINT) pop() == (MYINT) pop());
}

static void noteqfunc()
{
  push((MYINT) pop() != (MYINT) pop());
}

static void gtfunc()
{
  push((MYINT) pop() < (MYINT) pop());
}

static void ltfunc()
{
  push((MYINT) pop() > (MYINT) pop());
}

static void gtefunc()
{
  push((MYINT) pop() <= (MYINT) pop());
}

static void ltefunc()
{
  push((MYINT) pop() >= (MYINT) pop());
}

// assertions
static void assertfunc()
{
  MYINT truth = pop();
  if (!truth) {
      printf("ASSERT FAIL!\n");
  }
}

// true/false syntactic sugar
static void truefunc()
{
  push(-1);
}

static void falsefunc()
{
  push(0);
}
