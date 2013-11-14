#include "expressions.h"
#include "array.h"
#include "statements.h"

// else may be null
StmtNodeContainer *new_if_node(ExpNode *condition, StmtNodeContainer *then_stmt, 
    StmtNodeContainer *else_stmt) {
    return NULL;
}

// body may be null
StmtNodeContainer *new_while_node(ExpNode *condition, StmtNodeContainer *body) {
    return NULL;
}

// all arguments may be null
StmtNodeContainer *new_for_node(ExpNode *initialization, ExpNode *condition, 
    ExpNode *loop_expression, StmtNodeContainer *body) {
    return NULL;
}

// expr may be null
StmtNodeContainer *new_return_node(ExpNode *expr) {
    return NULL;
}

StmtNodeContainer *new_assignment_node(ExpNode *expr) {
    return NULL;
}

StmtNodeContainer *new_invocation_node(ExpNode *expr) {
    return NULL;
}

// stmts may be null
StmtNodeContainer *new_block_node(Array *stmts) {
    return NULL;
}

Array *stmt_list_new(StmtNodeContainer *stmt) {
    return NULL;
}

void stmt_list_insert(Array *stmts, StmtNodeContainer *stmt) {
}
