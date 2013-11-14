#ifndef _EXPRESSIONS_H_
#define _EXPRESSIONS_H_
#include "array.h"
#include "env.h"
typedef struct ExpNode {
    enum SymType return_type;
    int is_array;
    char * op; // usually operator, sometimes id
    struct ExpNode *lhs;
    struct ExpNode *rhs;
    enum {
        CONSTANT_EXPNODE,
        BOOLEAN_EXPNODE,
        COMP_EXPNODE,
        ARITHMETIC_EXPNODE,
        INVOCATION_EXPNODE,
        ID_EXPNODE,
        ASSIGNMENT_EXPNODE
    } node_type;
    Array *expns; // used only for invocation nodes
    struct ExpNode *index; // used only for id nodes
} ExpNode;

ExpNode *new_int_expnode();
ExpNode *new_float_expnode();
//new_char_expnode(); not in grammar?

ExpNode *new_boolean_expnode(char *op, ExpNode *lhs, ExpNode *rhs);
ExpNode *new_comparison_expnode(char *op, ExpNode *lhs, ExpNode *rhs);
ExpNode *new_arithmetic_expnode(char *op, ExpNode *lhs, ExpNode *rhs);
ExpNode *new_invocation_expnode(char *fn_id, Array *expnx, int should_be_void);
ExpNode *new_id_expnode(char *id, ExpNode *index);
ExpNode *new_assignment_expnode(ExpNode *lhs, ExpNode *rhs);

Array *expr_list_new(ExpNode * node);
Array *expr_list_insert(Array *exprx, ExpNode * node);

void validate_boolean_expression(ExpNode *node);
void validate_return_statement(ExpNode *node);

void expr_free(ExpNode *node);
#endif
