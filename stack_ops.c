/* stack operations */
static void push(MYFLT a)
{
  if (data_stack_ptr >= (sizeof(data_stack) / sizeof(*data_stack))) {
    printf("stack overflow\n");
    data_stack_ptr = 0;
  }
  data_stack[data_stack_ptr++] = a;
}

static MYFLT pop()
{
  if (data_stack_ptr <= 0) {
    printf("stack underflow! ");
    data_stack_ptr = 0;
    return 0;
  }
  return data_stack[--data_stack_ptr];
}

static void dropfunc()
{
  --data_stack_ptr;
}

static void dupfunc()
{
  if (data_stack_ptr < 1) {
    printf("stack underflow! ");
    return;
  }
  MYFLT val = data_stack[data_stack_ptr - 1];
  push(val);
}

static void swapfunc()
{
  if (data_stack_ptr < 2) {
    printf("stack underflow! ");
    return;
  }
  MYFLT val1 = pop();
  MYFLT val2 = pop();
  push(val1);
  push(val2); 
}

static void overfunc()
{
  if (data_stack_ptr < 2) {
    printf("stack underflow! ");
    return;
  }
  push(data_stack[data_stack_ptr - 2]);
}

static void rotfunc()
{
  if (data_stack_ptr < 3) {
    printf("stack underflow! ");
    return;
  }
  MYFLT c = pop();
  MYFLT b = pop();
  MYFLT a = pop();
  push(b);
  push(c);
  push(a);
}

static void nipfunc()
{
  if (data_stack_ptr < 2) {
    printf("stack underflow! ");
    return;
  }
  MYFLT b = pop();
  pop();
  push(b);
}

static void tuckfunc()
{
  if (data_stack_ptr < 2) {
    printf("stack underflow! ");
    return;
  }
  MYFLT val1 = pop();
  MYFLT val2 = pop();
  push(val1);
  push(val2);
  push(data_stack[data_stack_ptr - 2]);
}

static void drop2func()
{
  --data_stack_ptr;
  --data_stack_ptr;
}

static void dup2func()
{
  if (data_stack_ptr < 2) {
    printf("stack underflow! ");
    return;
  }
  MYFLT val1 = data_stack[data_stack_ptr - 2];
  MYFLT val2 = data_stack[data_stack_ptr - 1];
  push(val1);
  push(val2);
}

static void swap2func()
{
  if (data_stack_ptr < 4) {
    printf("stack underflow! ");
    return;
  }
  MYFLT val1 = pop();
  MYFLT val2 = pop();
  MYFLT val3 = pop();
  MYFLT val4 = pop();
  push(val2);
  push(val1);
  push(val4);
  push(val3); 
}

static void over2func()
{
  if (data_stack_ptr < 4) {
    printf("stack underflow! ");
    return;
  }
  push(data_stack[data_stack_ptr - 4]);
  push(data_stack[data_stack_ptr - 4]);
}

static void rot2func()
{
  // ( 6 5 4 3 2 1 -- 4 3 2 1 6 5 )
  if (data_stack_ptr < 6) {
    printf("stack underflow! ");
    return;
  }
  MYFLT val1 = pop();
  MYFLT val2 = pop();
  MYFLT val3 = pop();
  MYFLT val4 = pop();
  MYFLT val5 = pop();
  MYFLT val6 = pop();
  push(val4);
  push(val3);
  push(val2);
  push(val1);
  push(val6);
  push(val5); 
}
