#include <stdio.h>
#include <stdlib.h>
#include "expressions.h"
#include "semantics.h"
#include "env.h"
#include "array.h"
extern Env *current_scope; 
extern int current_return_type; 

int resolve_types(enum SymType t1, enum SymType t2) {
    if (t1 == t2)
        return t1;
    if (t1 == TYPE_INT && t2 == TYPE_CHAR)
        return TYPE_INT; 
    if (t2 == TYPE_INT && t1 == TYPE_CHAR)
        return TYPE_INT; 
    return -1;
}

ExpNode *_new_expnode(enum SymType return_type, char *op, ExpNode *lhs, 
    ExpNode *rhs, int is_array, int node_type) {
    ExpNode *node = calloc(1, sizeof(ExpNode));
    node->return_type = return_type;
    node->op = op;
    node->lhs = lhs;
    node->rhs = rhs;
    node->is_array = is_array;
    node->node_type = node_type;
    return node;
}

ExpNode *new_int_expnode(int val) {
    ExpNode *node = _new_expnode(TYPE_INT, NULL, NULL, NULL, 0, CONSTANT_EXPNODE);
    node->int_val = val;
    return node;
}

ExpNode *new_float_expnode(double val) {
    ExpNode *node = _new_expnode(TYPE_FLOAT, NULL, NULL, NULL, 0, CONSTANT_EXPNODE);
    node->float_val = val;
    return node;
}

ExpNode *new_boolean_expnode(char *op, ExpNode *lhs, ExpNode *rhs) {
    if (rhs == NULL) {
        if (lhs->return_type != TYPE_BOOL)
            print_error("Operand of unary boolean expression %s "
            "is not type-compatible with bool.", op); 
    } else {
        char bad_side = 0;
        if (lhs->return_type != TYPE_BOOL)
            bad_side |= 1;
        if (rhs->return_type != TYPE_BOOL)
            bad_side |= 2;
        if (bad_side != 0) {
            if (bad_side == 3)
                print_error("Neither side of boolean expression %s"
                " is type-compatible with bool.", op); 
            else
                print_error("%s-hand side of boolean expression %s"
                " is not type-compatible with bool.", 
                bad_side == 1 ? "Left" : "Right", op); 
        }
    }
    return _new_expnode(TYPE_BOOL, op, lhs, rhs, 0, BOOLEAN_EXPNODE);
}

ExpNode *new_comparison_expnode(char *op, ExpNode *lhs, ExpNode *rhs) {
    if (resolve_types(lhs->return_type, rhs->return_type) == -1) {
        print_error("Left- and right-hand sides of comparison "
        "expression %s (%s and %s, respecively) are not type-compatible.",
        op, _type_str(lhs->return_type), 
        _type_str(rhs->return_type));
    }
    return _new_expnode(TYPE_BOOL, op, lhs, rhs, 0, COMP_EXPNODE);
}

ExpNode *new_arithmetic_expnode(char *op, ExpNode *lhs, ExpNode *rhs) {
    enum SymType type;
    if (rhs == NULL) {
        type = lhs->return_type;
        if (type != TYPE_CHAR && type != TYPE_INT && type != TYPE_FLOAT) {
            print_error("Operand of unary arithmetic expression %s"
            " is not type-compatible with char, int, or float. Proceeding with "
            "the assuption that it is an int.", op); 
            type = TYPE_INT;
        }
    } else {
        type = resolve_types(lhs->return_type, rhs->return_type);
        if (type == -1) {
            print_error("Left- and right-hand sides of arithmetic "
            "expression %s (%s and %s, respecively) are not type-compatible. "
            "Proceeding with the assumption that the result of the expression "
            "is %s.", op, _type_str(lhs->return_type), 
            _type_str(rhs->return_type), _type_str(lhs->return_type));
            type = lhs->return_type;
        }
    }
    return _new_expnode(type, op, lhs, rhs, 0, ARITHMETIC_EXPNODE);
}

void _verify_types(char *fn_id, Array *tx, Array *expnx) {
    if (tx->length != expnx->length) {
        print_error("Invocation of function %s has too %s "
        "arguments.", fn_id, 
        tx->length > expnx->length ? "few" : "many");
        return;
    }

    int i;
    for (i = 0; i < tx->length; i++) {
        enum SymType argument_type = *((enum SymType *) array_get(tx, i));
        ExpNode *node = *((ExpNode **) array_get(expnx, i));
        if (resolve_types(argument_type, node->return_type) == -1) {
            // TODO: type promotion of parameters
            print_error("Parameter %d of invocation of function "
            "%s is of type %s, but argument %d of function %s is of type %s. "
            "These types are incompatible.", i, fn_id, 
            _type_str(node->return_type), i, fn_id, _type_str(argument_type));
        }
    }
}

ExpNode *new_invocation_expnode(char *fn_id, Array *expnx, int should_be_void) {
    if (!expnx)
        expnx = array_new(0, sizeof(ExpNode *));
    Symbol *sym = Env_get(current_scope, fn_id);
    if (!sym)
        sym = Env_get_prot(current_scope, fn_id);
    if (!sym)
        print_error("Attempt to invoke undeclared function %s. "
        "Proceeding with the assumption that the return type of the function "
        "is int.", fn_id);
    else if (sym->type != TYPE_FN && sym->type != TYPE_FN_PROT)
        print_error("Attempt to invoke function %s, but %s is of "
        "type %s. Proceeding with the assumption that the return type of the "
        "function is int.", fn_id, fn_id, _type_str(sym->type));
    else if (sym->return_type == TYPE_VOID && !should_be_void)
        print_error("Attempt to use void function %s in "
        "expression. Continuing with the assuption that the return type of %s "
        "is int.", fn_id, fn_id);
    else if (sym->return_type != TYPE_VOID && should_be_void)
        print_error("Attempt to invoke non-void function %s in "
        "statement. Continuing with the assuption that the return type of %s "
        "is void.", fn_id, fn_id);
    else {
        _verify_types(fn_id, sym->type_list, expnx);
        ExpNode *node = _new_expnode(sym->return_type, fn_id, NULL, NULL, 0, 
            INVOCATION_EXPNODE);
        node->expns = expnx;
        node->symbol = sym;
        return node;
    }
    // this won't make it to code generation anyway.
    return _new_expnode(TYPE_INT, NULL, NULL, NULL, 0, INVOCATION_EXPNODE); 
}

ExpNode *new_id_expnode(char *id, ExpNode *index) {
    Symbol *sym = Env_get(current_scope, id);
    if (!sym)
        print_error("Variable %s used before declaration. "
        "Proceeding with the assumption that the type of %s is int.", 
         id, id);
    else if (sym->type != TYPE_CHAR && sym->type != TYPE_INT && 
        sym->type != TYPE_FLOAT)
        print_error("Variable %s is of a type than cannot be used"
        " as part of an expression.", id);
    else {
        if (index && sym->is_array == 0)
            print_error("Index used with variable %s, but %s is "
            "not a pointer. Continuing with the type of %s.", id, 
            id, id);
        if (index && resolve_types(index->return_type, TYPE_INT) == -1)
            print_error("Index used with variable %s is not "
            "type-compatible with int.", id);
        int is_array = sym->is_array == (index == NULL);
        ExpNode *node = _new_expnode(sym->type, id, NULL, NULL, is_array, 
            ID_EXPNODE);
        node->index = index;
        node->symbol = sym;
        return node;
    }
    // won't make it to code generation
    return _new_expnode(TYPE_INT, NULL, NULL, NULL, 0, ID_EXPNODE);
}

ExpNode *new_assignment_expnode(ExpNode *lhs, ExpNode *rhs) {
    int type = resolve_types(lhs->return_type, rhs->return_type);
    if (lhs->is_array != rhs->is_array 
        || type == -1)
        print_error("Left- and right-hand sides of assignment are not "
        "type-compatible");    
    return _new_expnode(type, NULL, lhs, rhs, 0, ASSIGNMENT_EXPNODE);
}

Array *expr_list_insert(Array *exprx, ExpNode * node) {
    array_append(exprx, &node);
    return exprx;
}

Array *expr_list_new(ExpNode * node) {
    Array *exprx = array_new(0, sizeof(ExpNode *));
    expr_list_insert(exprx, node);
    return exprx;
}

void validate_boolean_expression(ExpNode *node) {
    if (node && node->return_type != TYPE_BOOL) 
        print_error("Expression used as conditional does not return type "
        "boolean.");
}

void validate_return_statement(ExpNode *node) {
    if (node && resolve_types(node->return_type, current_return_type) == -1)
        print_error("Return statement if of type %s, but return type of "
        "enclosing function is %s", _type_str(node->return_type),
        _type_str(current_return_type));
    else if (!node && current_return_type != TYPE_VOID)
        print_error("Empty return statement in a non-void function");
    current_scope->has_return_statement = 1; // hack
}

void expr_free(ExpNode *node) {
    if (node->lhs)
        expr_free(node->lhs);
    if (node->rhs)
        expr_free(node->rhs);
    if (node->expns) {
        array_free(node->expns);
        free(node->expns);
    }
    if (node->index)
        expr_free(node->index);
    free(node);
}
