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
  push((MYFLT) pop() == (MYFLT) pop());
}

static void noteqfunc()
{
  push((MYFLT) pop() != (MYFLT) pop());
}

static void gtfunc()
{
  push((MYFLT) pop() < (MYFLT) pop());
}

static void ltfunc()
{
  push((MYFLT) pop() > (MYFLT) pop());
}

static void gtefunc()
{
  push((MYFLT) pop() <= (MYFLT) pop());
}

static void ltefunc()
{
  push((MYFLT) pop() >= (MYFLT) pop());
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
