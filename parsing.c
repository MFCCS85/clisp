#include "error_handling.h"
#include "mpc.h"
#include <stdlib.h>

#ifdef _WIN32

static char buffer[2048];

char *readline(char *prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char *cpy = malloc(strlen(buffer) + 1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy) - 1] = '\0';
  return cpy;
}

void add_history(char *unused) {}

#else
#include <editline/history.h>
#include <editline/readline.h>
#endif

long power(long x, long y) {
  long result = 1;
  for (long i = 0; i < y; i++) {
    result = result * x;
  }
  return result;
}

long _max(long x, long y) {
  if (x > y)
    return x;
  else
    return y;
}

long _min(long x, long y) {
  if (x < y)
    return x;
  else
    return y;
}

lval eval_op(lval x, char *op, lval y) {
  if (x.type == LVAL_ERR) {
    return x;
  }
  if (y.type == LVAL_ERR) {
    return y;
  }

  if (strcmp(op, "+") == 0 || strcmp(op, "add") == 0) {
    return lval_num(x.num + y.num);
  }
  if (strcmp(op, "-") == 0 || strcmp(op, "sub") == 0) {
    return lval_num(x.num - y.num);
  }
  if (strcmp(op, "*") == 0 || strcmp(op, "mul") == 0) {
    return lval_num(x.num * y.num);
  }
  if (strcmp(op, "/") == 0 || strcmp(op, "div") == 0) {
    return y.num == 0 ? lval_err(LERR_DIV_ZRO) : lval_num(x.num / y.num);
  }
  if (strcmp(op, "%") == 0 || strcmp(op, "mod") == 0) {
    return lval_num(x.num / y.num);
  }
  if (strcmp(op, "^") == 0 || strcmp(op, "exp") == 0) {
    return lval_num(x.num / y.num);
  }
  if (strcmp(op, "max") == 0) {
    return lval_num(x.num / y.num);
  }
  if (strcmp(op, "min") == 0) {
    return lval_num(x.num / y.num);
  }
  return lval_num(0);
}

lval eval(mpc_ast_t *t) {
  if (strstr(t->tag, "number")) {
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
  }

  char *op = t->children[1]->contents;
  lval x = eval(t->children[2]);

  if (strcmp(op, "-") == 0 && t->children_num == 4) {
    return lval_num(-eval(t->children[2]).num);
  }

  int i = 3;
  while (strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  }

  return x;
}

// bonus
long num_leaves(mpc_ast_t *t) {
  if (strstr(t->tag, "number"))
    return 1;

  int count = 0;
  for (long i = 0; i < t->children_num; i++) {
    count += num_leaves(t->children[i]);
  }

  return count;
}

long num_branches(mpc_ast_t *t) {
  if (t->children_num == 0)
    return 0;

  long count = 1;
  for (int i = 0; i < t->children_num; i++) {
    count += num_branches(t->children[i]);
  }

  return count;
}

int main(int argc, char **argv) {

  mpc_parser_t *Number = mpc_new("number");
  mpc_parser_t *Operator = mpc_new("operator");
  mpc_parser_t *Expr = mpc_new("expr");
  mpc_parser_t *Lispy = mpc_new("lispy");

  mpca_lang(MPCA_LANG_DEFAULT, "                  \
      number   : /-?[0-9]+/ ;                                     \
      operator : '+'   | '-'   | '*'   | '/'   | '%'   | '^' |    \
                 /add/ | /sub/ | /mul/ | /div/ | /mod/ |/exp/|    \
                 /max/ | /min/ | /- /;                            \
      expr     : <number> | '(' <operator> <expr>+ ')' ;          \
      lispy    : /^/<operator> <expr>+ /$/;          \
    ",
            Number, Operator, Expr, Lispy);
  puts("Lispy Version 0.0.0.0.1");
  puts("Press Ctrl+c to exit\n");

  while (1) {
    char *input = readline("lispy> ");
    add_history(input);

    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lispy, &r)) {
      lval result = eval(r.output);
      printf("RESULT --->");
      lval_print(result);
      printf("\n");
      printf("LEAVES ---> %li\n", num_leaves(r.output));
      printf("BRANCHES ---> %li\n", num_branches(r.output));
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    free(input);
  }

  mpc_cleanup(4, Number, Operator, Expr, Lispy);

  return 0;
}
