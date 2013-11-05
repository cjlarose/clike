#include <stdio.h>
#include "expressions.h"
#include "semantics.h"
#include "env.h"
#include "array.h"
extern Env *current_scope; 
extern int line_num;

int compatible_types(enum SymType t1, enum SymType t2) {
    if (t1 == t2)
        return true;
    if (t1 == TYPE_INT && t2 == TYPE_CHAR)
        return true;
    if (t1 == TYPE_INT && t2 == TYPE_CHAR)
        return true;
    return false;
}

ExpNode *new_int_expnode() {
    return NULL;
}
ExpNode *new_float_expnode() {
    return NULL;
}

ExpNode *new_boolean_expnode(char *op, ExpNode *lhs, ExpNode *rhs) {
    return NULL;
}

ExpNode *new_arithmetic_expnode(char *op, ExpNode *lhs, ExpNode *rhs) {
    return NULL;
}

ExpNode *new_invocation_expnode(char *fn_id, Array *expnx) {
    return NULL;
}

ExpNode *new_id_expnode(char *id, int has_index) {
    return NULL;
}
