#ifndef _EXPRESSIONS_H_
#define _EXPRESSIONS_H_
#include "array.h"
#include "env.h"
typedef struct ExpNode {
    enum SymType return_type;
    char * op;
    struct ExpNode *lhs;
    struct ExpNode *rhs;
} ExpNode;

ExpNode *new_int_expnode();
ExpNode *new_float_expnode();
//new_char_expnode(); not in grammar?

ExpNode *new_boolean_expnode(char *op, ExpNode *lhs, ExpNode *rhs);
ExpNode *new_arithmetic_expnode(char *op, ExpNode *lhs, ExpNode *rhs);
ExpNode *new_invocation_expnode(char *fn_id, Array *expnx);
ExpNode *new_id_expnode(char *id, int has_index);
#endif