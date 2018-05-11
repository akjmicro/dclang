/* logical words */
static void andfunc()
{
  push((unsigned int) pop() & (unsigned int) pop());
}

static void orfunc()
{
  push((unsigned int) pop() | (unsigned int) pop());
}

static void xorfunc()
{
  push((unsigned int) pop() ^ (unsigned int) pop());
}

static void notfunc()
{
  push(~(unsigned int) pop());
}

/* comparison booleans */
static void eqfunc()
{
  long double num2 = pop(); 
  push(pop() == num2);
}

static void noteqfunc()
{
  long double num2 = pop();
  push(pop() != num2);
}

static void gtfunc()
{
  long double num2 = pop(); 
  push(pop() > num2);
}

static void ltfunc()
{
  long double num2 = pop(); 
  push(pop() < num2);
}

static void gtefunc()
{
  long double num2 = pop(); 
  push(pop() >= num2);
}

static void ltefunc()
{
  long double num2 = pop(); 
  push(pop() <= num2);
}
