#include <stdio.h>
#include "expressions.h"
#include "semantics.h"
#include "env.h"
#include "array.h"
extern Env *current_scope; 
extern int line_num;

int resolve_types(enum SymType t1, enum SymType t2) {
    if (t1 == t2)
        return t1;
    if (t1 == TYPE_INT && t2 == TYPE_CHAR)
        return TYPE_INT; 
    if (t2 == TYPE_INT && t1 == TYPE_CHAR)
        return TYPE_INT; 
    return -1;
}

ExpNode *_new_expnode(enum SymType return_type, char *op, ExpNode *lhs, ExpNode *rhs) {
    ExpNode *node = malloc(sizeof(ExpNode));
    node->return_type = return_type;
    node->op = op;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

ExpNode *new_int_expnode() {
    return _new_expnode(TYPE_INT, NULL, NULL, NULL);
}

ExpNode *new_float_expnode() {
    return _new_expnode(TYPE_FLOAT, NULL, NULL, NULL);
}

ExpNode *new_boolean_expnode(char *op, ExpNode *lhs, ExpNode *rhs) {
    char bad_side = 0;
    if (lhs->return_type != TYPE_BOOL)
        bad_side |= 1;
    if (rhs->return_type != TYPE_BOOL)
        bad_side |= 2;
    if (bad_side != 0) {
        if (bad_side == 3)
            fprintf(stderr, "Line %d: Both sides of boolean expression %s are "
            "not type-compatible with bool.\n", line_num, op); 
        else
            fprintf(stderr, "Line %d: %s-hand side of boolean expression %s is "
            "not type-compatible with bool.\n", line_num, 
            bad_side == 1 ? "Left" : "Right", op); 
    }
    return _new_expnode(TYPE_BOOL, op, lhs, rhs);
}

ExpNode *new_arithmetic_expnode(char *op, ExpNode *lhs, ExpNode *rhs) {
    // if rhs is null
    //enum SymType type = resolve_type(lhs, rhs);
    //return _new_expnode(type, op, lhs, rhs);
    return NULL;
}

ExpNode *new_invocation_expnode(char *fn_id, Array *expnx) {
    // validate expnx
    // get return type
    //return _new_expnode(return_type, NULL, NULL, NULL);
    return NULL;
}

ExpNode *new_id_expnode(char *id, int has_index) {
    // get return type
    return NULL;
}
