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

#define IBUFSIZE 128
char buf[IBUFSIZE];
int bufused;

static int return_stack[32];
static int return_stack_ptr;
static int loop_counter[3];
static int loop_counter_ptr;
// our tokens to be interpreted go here:
char tokens[MAXWORD][MAXLEN];
static int token_ptr;
static int ntokens = 0;

#include "stack_ops.c"
#include "logic_ops.c"
#include "math_ops.c"
#include "branch_ops.c"
#include "time_ops.c"
#include "output_ops.c"
#include "string_ops.c"
#include "variable_ops.c"
#include "operators.c"

static void stack_machine(const char *argument)
{
    char *endPointer = 0;
    long double d;
    const struct primitive *pr = primitives;

    if (argument == 0) {
        printfunc();
        return;
    } 

    /* search dictionary (list, not hash) entry */
    while (pr->name != 0) {
        if (strcmp(pr->name, argument) == 0) {
            (*(pr->function)) ();
            return;
        }
        pr++;
    }

    /* next, try to convert to a number */
    d = strtod(argument, &endPointer);
    if (endPointer != argument) {
        push(d);
        return;
    }

    /* nothing found, we've reached an error condition, so report
    the situation and reset the stack */
    data_stack_ptr = 0;
    printf("%s -- syntax error.\n", argument); 
    return;    
}

void add_to_buf(char ch) { if(bufused < IBUFSIZE - 1) buf[bufused++] = ch; }
char *buf2str()          { buf[bufused++] = '\0'; return strdup(buf); }

char *get_token() {
    int ch;
    bufused = 0;
    /* skip leading spaces and comments */
    while (1) {
        /* skip leading space */
        do {
            if((ch = fgetc(stdin)) == EOF) exit(0);
        } while(isspace(ch));
        /* if we are starting a comment: */
        if (strchr("#", ch)) {
            /* go to the end of the line */
            do {
                if((ch = fgetc(stdin)) == EOF) exit(0);
            } while(! strchr("\n", ch));
        } else {
            add_to_buf(ch);
            break;
        }
    }
    /* grab all the next non-whitespace characters */
    while (1) {
        /* check again for EOF */
        if ((ch = fgetc(stdin)) == EOF) exit(0);
        if (isspace(ch)) {
            ungetc(ch, stdin);
            return buf2str();
        }
        add_to_buf(ch);
    }
}


int main(int argc, char **argv)
{ 
    if (argc <= 1) {
        printf("Welcome to dclang! Aaron Krister Johnson, 2018\n");
        printf("Make sure to peruse README.md to get your bearings!\n");
        while (1) {
            /* get next input token */
            char *token;
            token = get_token();
            strcpy(tokens[ntokens++], token);
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
