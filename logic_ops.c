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
  MYINT num2 = pop(); 
  push(pop() == num2);
}

static void noteqfunc()
{
  MYINT num2 = pop();
  push(pop() != num2);
}

static void gtfunc()
{
  MYINT num2 = pop(); 
  push(pop() > num2);
}

static void ltfunc()
{
  MYINT num2 = pop(); 
  push(pop() < num2);
}

static void gtefunc()
{
  MYINT num2 = pop(); 
  push(pop() >= num2);
}

static void ltefunc()
{
  MYINT num2 = pop(); 
  push(pop() <= num2);
}

// assertions
static void assertfunc()
{
  MYINT truth = pop();
  if (!truth) {
      printf("ASSERT FAIL!\n");
  }
}
