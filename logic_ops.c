/* logical words */
static void andfunc()
{
  push((unsigned MYINT) pop() & (unsigned MYINT) pop());
}

static void orfunc()
{
  push((unsigned MYINT) pop() | (unsigned MYINT) pop());
}

static void xorfunc()
{
  push((unsigned MYINT) pop() ^ (unsigned MYINT) pop());
}

static void notfunc()
{
  push(~(unsigned MYINT) pop());
}

/* comparison booleans */
static void eqfunc()
{
  MYFLT num2 = pop(); 
  push(pop() == num2);
}

static void noteqfunc()
{
  MYFLT num2 = pop();
  push(pop() != num2);
}

static void gtfunc()
{
  MYFLT num2 = pop(); 
  push(pop() > num2);
}

static void ltfunc()
{
  MYFLT num2 = pop(); 
  push(pop() < num2);
}

static void gtefunc()
{
  MYFLT num2 = pop(); 
  push(pop() >= num2);
}

static void ltefunc()
{
  MYFLT num2 = pop(); 
  push(pop() <= num2);
}

// assertions
static void assertfunc()
{
  MYFLT truth = pop();
  if (!truth) {
      printf("ASSERT FAIL!\n");
  }
}
