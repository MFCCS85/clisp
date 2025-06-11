enum { LVAL_NUM, LVAL_ERR };
enum { LERR_DIV_ZRO, LERR_BAD_OP, LERR_BAD_NUM };

typedef struct {
  int type;
  long num;
  int err;
} lval;
