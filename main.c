/* dclang, an RPN calculator that has dreams of being a full-blown
Turing-complete programming language, a descendant of FORTH in minimalism
and philosophy.  Born on 2018-05-05 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#define DELIM " "
#define MAXWORD 16384
#define MAXLEN 20

static unsigned int truth_val;
static unsigned int cond_state;
static unsigned int all_true;
static int return_stack[32];
static int return_stack_ptr;
static int loop_counter[3];
static int loop_counter_ptr;
static unsigned int exit_stat;
// our tokens to be interpreted go here:
char tokens[MAXWORD][MAXLEN];
static int token_ptr;
static int ntokens = 0;

#include "stack_ops.c"
#include "logic_ops.c"
#include "math_ops.c"
#include "loop_ops.c"
#include "time_ops.c"
#include "output_ops.c"
#include "operator_hashing.c"

static void stack_machine(const char *argument)
{
  if (argument == 0) {
    printfunc();
    return;
  } 

  /* see if we are in a conditional word: */
  if (strcmp("endif", argument) == 0) {
    cond_state = cond_state >> 1;
    all_true = all_true >> 1;
    return;
  }
  if (strcmp("if", argument) == 0) {
    all_true = (all_true << 1) + 1;
    truth_val = (unsigned int) pop();
    cond_state = (cond_state << 1) + truth_val;
    return; 
  }
  if (strcmp("else", argument) == 0) {
    if ((cond_state & 0x1) == 1) {
      --cond_state;
    } else {
      ++cond_state;
    }
    return;
  }
  
  if ((cond_state & all_true) == all_true) {
    /* try to convert to a number first */
    char *endPointer = 0;
    long double d;
    d = strtod(argument, &endPointer);
    if (endPointer != argument) {
      push(d);
      return;
    }
    /* next, search via hash for operator */
    int hash = get_hash((unsigned char *) argument);
    if (operators[hash]) {
      (*(operators[hash])) ();
      return;
    }
    /* nothing found, we've reached an error condition, so report
    the situation and reset the stack */
    data_stack_ptr = 0;
    printf("%s -- syntax error.\n", argument); 
  }
  return;  
}

int main(int argc, char **argv)
{ 
  populate_operators();

  if (argc <= 1) {    
    while (1) {
      char buff[BUFSIZ];
      char *newline_p;
      /* get a bit of input text */
      if (fgets(buff, sizeof buff, stdin) != NULL) {
        if ((newline_p = strchr(buff, '\n')) != NULL) {
          *newline_p = '\0';
        }
        char *sep = strtok(buff, DELIM);
        while (sep != NULL ) {
          strcpy(tokens[ntokens++], sep);
          sep = strtok(NULL, DELIM);
        }
      }
      /* interpret what hasn't been interpreted yet */
      while (token_ptr < ntokens) {
        stack_machine(tokens[token_ptr++]);
      }
    }
  }
  else {
    if (*argv[1]=='-')
      printf("you need help!\n");
    while (argc >= 2) {
      stack_machine(argv[1]);
      argv++;
      argc--;
    }
  }

  stack_machine(0);
  return EXIT_SUCCESS;
}
