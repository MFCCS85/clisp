enum { LVAL_NUM, LVAL_ERR };
enum { LERR_DIV_ZRO, LERR_BAD_OP, LERR_BAD_NUM };

typedef struct {
  int type;
  double num;
  int err;
} lval;

lval lval_num(long x);
lval lval_err(int x);
void lval_print(lval v);
