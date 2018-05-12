/* looping */
static void beginfunc()
{
  return_stack[return_stack_ptr++] = token_ptr;
  loop_counter_ptr++;
}

static void againfunc()
{
  if (exit_stat == 0) {
    loop_counter[loop_counter_ptr - 1] += 1;
    token_ptr = return_stack[return_stack_ptr - 1];
  } else {
    /* reset exit flag */
    exit_stat = 0;
  }
  /* also must decrement the conditional stack pointer, the assumption will
  be that an if statement was in there */
  //--cond_stack_ptr;*/
  /* keep it from being less than zero, ever: */
  //cond_stack_ptr = cond_stack_ptr < 0 ? 0 : cond_stack_ptr;
}

static void exitfunc()
{
  loop_counter[--loop_counter_ptr] = 0;
  --return_stack_ptr;
  /* set exit flag */
  exit_stat = 1;
}

static void ifunc()
{
  push(loop_counter[0]);
}

static void jfunc()
{
  push(loop_counter[1]);
}

static void kfunc()
{
  push(loop_counter[2]);
}
