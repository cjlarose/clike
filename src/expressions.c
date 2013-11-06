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
    if (rhs == NULL) {
        if (rhs->return_type != TYPE_BOOL)
            fprintf(stderr, "Line %d: Operand of unary boolean expression %s "
            "is not type-compatible with bool.\n", line_num, op); 
    } else {
        char bad_side = 0;
        if (lhs->return_type != TYPE_BOOL)
            bad_side |= 1;
        if (rhs->return_type != TYPE_BOOL)
            bad_side |= 2;
        if (bad_side != 0) {
            if (bad_side == 3)
                fprintf(stderr, "Line %d: Neither side of boolean expression %s"
                " is type-compatible with bool.\n", line_num, op); 
            else
                fprintf(stderr, "Line %d: %s-hand side of boolean expression %s"
                " is not type-compatible with bool.\n", line_num, 
                bad_side == 1 ? "Left" : "Right", op); 
        }
    }
    return _new_expnode(TYPE_BOOL, op, lhs, rhs);
}

ExpNode *new_comparison_expnode(char *op, ExpNode *lhs, ExpNode *rhs) {
    if (resolve_types(lhs->return_type, rhs->return_type) == -1) {
        fprintf(stderr, "Line %d: Left- and right-hand sides of comparison "
        "expression %s (%s and %s, respecively) are not type-compatible.\n",
        line_num, op, _type_str(lhs->return_type), 
        _type_str(rhs->return_type));
    }
    return _new_expnode(TYPE_BOOL, op, lhs, rhs);
}

ExpNode *new_arithmetic_expnode(char *op, ExpNode *lhs, ExpNode *rhs) {
    enum SymType type;
    if (rhs == NULL) {
        type = lhs->return_type;
        if (type != TYPE_CHAR && type != TYPE_INT && type != TYPE_FLOAT) {
            fprintf(stderr, "Line %d: Operand of unary arithmetic expression %s"
            " is not type-compatible with char, int, or float. Proceeding with "
            "the assuption that it is an int.\n", line_num, op); 
            type = TYPE_INT;
        }
    } else {
        type = resolve_types(lhs->return_type, rhs->return_type);
        if (type == -1) {
            fprintf(stderr, "Line %d: Left- and right-hand sides of arithmetic "
            "expression %s (%s and %s, respecively) are not type-compatible. "
            "Proceeding with the assumption that the result of the expression "
            "is %s.\n", line_num, op, _type_str(lhs->return_type), 
            _type_str(rhs->return_type), _type_str(lhs->return_type));
            type = lhs->return_type;
        }
    }
    return _new_expnode(type, op, lhs, rhs);
}

void _verify_types(char *fn_id, Array *tx, Array *expnx) {
    if (tx->length != expnx->length) {
        fprintf(stderr, "Line %d: Invocation of function %s has too %s "
        "arguments.\n", line_num, fn_id, 
        tx->length > expnx->length ? "few" : "many");
    }

    int i;
    for (i = 0; i < tx->length; i++) {
        enum SymType argument_type = *((enum SymType *) Array_get(tx, i));
        ExpNode *node = Array_get(expnx, i);
        if (resolve_types(argument_type, node->return_type) == -1) {
            // TODO: type promotion of parameters
            fprintf(stderr, "Line %d: Parameter %d of invocation of function "
            "%s is of type %s, but argument %d of function %s is of type %s. "
            "These types are incompatible.\n", line_num, i, fn_id, 
            _type_str(node->return_type), i, fn_id, _type_str(argument_type));
        }
    }
}

ExpNode *new_invocation_expnode(char *fn_id, Array *expnx, int should_be_void) {
    if (!expnx)
        expnx = Array_init(0, sizeof(ExpNode *));
    Symbol *sym = Env_get(current_scope, fn_id);
    if (!sym)
        fprintf(stderr, "Line %d: Attempt to invoke undeclared function %s. "
        "Proceeding with the assumption that the return type of the function "
        "is int.\n", line_num, fn_id);
    else if (sym->type != TYPE_FN)
        fprintf(stderr, "Line %d: Attempt to invoke function %s, but %s is of "
        "type %s. Proceeding with the assumption that the return type of the "
        "function is int.\n", line_num, fn_id, fn_id, _type_str(sym->type));
    else if (sym->return_type == TYPE_VOID && !should_be_void)
        fprintf(stderr, "Line %d: Attempt to use void function %s in "
        "expression. Continuing with the assuption that the return type of %s "
        "is int.\n", line_num, fn_id, fn_id);
    else if (sym->return_type != TYPE_VOID && should_be_void)
        fprintf(stderr, "Line %d: Attempt to invoke non-void function %s in "
        "statement. Continuing with the assuption that the return type of %s "
        "is void.\n", line_num, fn_id, fn_id);
    else {
        _verify_types(fn_id, sym->type_list, expnx);
        return _new_expnode(sym->return_type, NULL, NULL, NULL);
    }
    return _new_expnode(TYPE_INT, NULL, NULL, NULL); // keep the party gooooin
}

ExpNode *new_id_expnode(char *id, ExpNode *index) {
    Symbol *sym = Env_get(current_scope, id);
    if (!sym)
        fprintf(stderr, "Line %d: Variable %s used before declaration. "
        "Proceeding with the assumption that the type of %s is int.\n", 
         line_num, id, id);
    else if (sym->type != TYPE_CHAR && sym->type != TYPE_INT && sym->type != TYPE_FLOAT)
        fprintf(stderr, "Line %d: Variable %s is of a type than cannot be used"
        " as part of an expression.\n", line_num, id);
    else {
        if (index && sym->is_array == 0)
            fprintf(stderr, "Line %d: Index used with variable %s, but %s is "
            "not a pointer. Continuing with the type of %s.\n", line_num, id, 
            id, id);
        if (index && resolve_types(index->return_type, TYPE_INT) == -1)
            fprintf(stderr, "Line %d: Index used with variable %s is not "
            "type-compatible with int.\n", line_num, id);
        return _new_expnode(sym->type, NULL, NULL, NULL);
    }
    return _new_expnode(TYPE_INT, NULL, NULL, NULL);
}

Array *expr_list_insert(Array *exprx, ExpNode * node) {
    Array_append(exprx, node);
    return exprx;
}

Array *expr_list_new(ExpNode * node) {
    Array *exprx = Array_init(0, sizeof(ExpNode *));
    expr_list_insert(exprx, node);
    return exprx;
}
