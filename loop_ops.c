/* looping */
static void dofunc()
{
  return_stack[return_stack_ptr++] = token_ptr;
  loop_counter_ptr++;
  //loop_stat = (loop_stat << 1) + 1;
}

static void exitfunc()
{
  loop_counter[--loop_counter_ptr] = 0;
  --return_stack_ptr;
  /* clear LSB loop flag */
  //loop_stat = loop_stat & 0xfe;
}

static void redofunc()
{
  if ((unsigned int) pop() != 0) {
    loop_counter[loop_counter_ptr - 1] += 1;
    token_ptr = return_stack[return_stack_ptr - 1];
  } else {
    exitfunc();
    /* reset exit flag */
    //loop_stat = loop_stat >> 1;
  }
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
