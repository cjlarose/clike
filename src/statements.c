#include <stdlib.h>
#include "expressions.h"
#include "array.h"
#include "statements.h"

// else may be null
StmtNodeContainer *new_if_node(ExpNode *condition, StmtNodeContainer *then_stmt, 
    StmtNodeContainer *else_stmt) {
    StmtNodeContainer *cont = malloc(sizeof(StmtNodeContainer));
    cont->type = IF_STMT;
    cont->node.if_stmt.condition = condition;
    cont->node.if_stmt.then_stmt = then_stmt;
    cont->node.if_stmt.else_stmt = else_stmt;
    return cont;
}

// body may be null
StmtNodeContainer *new_while_node(ExpNode *condition, StmtNodeContainer *body) {
    StmtNodeContainer *cont = malloc(sizeof(StmtNodeContainer));
    cont->type = WHILE_STMT;
    cont->node.while_stmt.condition = condition;
    cont->node.while_stmt.body = body;
    return cont;
}

// all arguments may be null
StmtNodeContainer *new_for_node(ExpNode *initialization, ExpNode *condition, 
    ExpNode *loop_expression, StmtNodeContainer *body) {
    StmtNodeContainer *cont = malloc(sizeof(StmtNodeContainer));
    cont->type = FOR_STMT;
    cont->node.for_stmt.initialization = initialization;
    cont->node.for_stmt.condition = condition;
    cont->node.for_stmt.loop_expression = loop_expression;
    cont->node.for_stmt.body = body;
    return cont;
}

// expr may be null
StmtNodeContainer *new_return_node(ExpNode *expr) {
    StmtNodeContainer *cont = malloc(sizeof(StmtNodeContainer));
    cont->type = RETURN_STMT;
    cont->node.return_stmt.expr = expr;
    return cont;
}

StmtNodeContainer *new_assignment_node(ExpNode *expr) {
    StmtNodeContainer *cont = malloc(sizeof(StmtNodeContainer));
    cont->type = ASSIGNMENT_STMT;
    cont->node.assg_stmt.expr = expr;
    return cont;
}

StmtNodeContainer *new_invocation_node(ExpNode *expr) {
    StmtNodeContainer *cont = malloc(sizeof(StmtNodeContainer));
    cont->type = INVOCATION_STMT;
    cont->node.invoc_stmt.expr = expr;
    return cont;
}

// stmts may be null
StmtNodeContainer *new_block_node(Array *stmts) {
    StmtNodeContainer *cont = malloc(sizeof(StmtNodeContainer));
    cont->type = BLOCK_STMT;
    cont->node.block_stmt.stmts = stmts;
    return cont;
}

void stmt_list_insert(Array *stmts, StmtNodeContainer *stmt) {
    array_append(stmts, stmt);
}

Array *stmt_list_new(StmtNodeContainer *stmt) {
    Array *arr = array_new(0, sizeof(StmtNodeContainer));
    stmt_list_insert(arr, stmt);
    return arr;
}

void stmt_free(StmtNodeContainer *stmt) {
    Array *stmts;
    switch (stmt->type) {
        case IF_STMT:
            expr_free(stmt->node.if_stmt.condition);
            stmt_free(stmt->node.if_stmt.then_stmt);
            if (stmt->node.if_stmt.else_stmt)
                stmt_free(stmt->node.if_stmt.else_stmt);
            break;
        case WHILE_STMT:
            expr_free(stmt->node.while_stmt.condition);
            if (stmt->node.while_stmt.body)
                stmt_free(stmt->node.while_stmt.body);
            break;
        case FOR_STMT:
            if (stmt->node.for_stmt.initialization)
                expr_free(stmt->node.for_stmt.initialization);
            if (stmt->node.for_stmt.condition)
                expr_free(stmt->node.for_stmt.condition);
            if (stmt->node.for_stmt.loop_expression)
                expr_free(stmt->node.for_stmt.loop_expression);
            if (stmt->node.for_stmt.body)
                stmt_free(stmt->node.for_stmt.body);
            break;
        case RETURN_STMT:
            if (stmt->node.return_stmt.expr)
                expr_free(stmt->node.return_stmt.expr);
            break;
        case ASSIGNMENT_STMT:
            expr_free(stmt->node.assg_stmt.expr);
            break;
        case INVOCATION_STMT:
            expr_free(stmt->node.invoc_stmt.expr);
            break;
        case BLOCK_STMT:
            stmts = stmt->node.block_stmt.stmts;
            if (stmts) {
                int i;
                for (i = 0; i < stmts->length; i++)
                    stmt_free(array_get(stmts, i));
                array_free(stmts);
            }
            break;
    }
}
