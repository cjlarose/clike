#ifndef _STATEMENTS_H_
#define _STATEMENTS_H_
#include "expressions.h"
#include "array.h"
enum StatementType {
    IF_STMT,
    WHILE_STMT,
    FOR_STMT,
    RETURN_STMT,
    ASSIGNMENT_STMT,
    INVOCATION_STMT,
    BLOCK_STMT
};

struct StmtNodeContainer;

typedef struct IfStatement {
    ExpNode *condition;
    struct StmtNodeContainer *then_stmt;
    struct StmtNodeContainer *else_stmt;
} IfStatement;

typedef struct WhileStatement {
    ExpNode *condition;
    struct StmtNodeContainer *body;
} WhileStatement;

typedef struct ForStatement {
    ExpNode *initialization;
    ExpNode *condition;
    ExpNode *loop_expression;
    struct StmtNodeContainer *body;
} ForStatement;

typedef struct ReturnStatement {
    ExpNode *expr;
} ReturnStatement;

typedef struct AssignmentStatement {
    ExpNode *expr;
} AssignmentStatement;

typedef struct InvocationStatement {
    ExpNode *expr;
} InvocationStatement;

typedef struct BlockStatement {
    Array *stmts;
} BlockStatement;

union StatementNode {
    IfStatement if_stmt;
    WhileStatement while_stmt;
    ForStatement for_stmt;
    ReturnStatement return_stmt;
    AssignmentStatement assg_stmt;
    InvocationStatement invoc_stmt;
    BlockStatement block_stmt;
};

typedef struct StmtNodeContainer {
    enum StatementType type;
    union StatementNode node;
} StmtNodeContainer;

StmtNodeContainer *new_if_node(ExpNode *condition, StmtNodeContainer *then_stmt, 
    StmtNodeContainer *else_stmt);
StmtNodeContainer *new_while_node(ExpNode *condition, StmtNodeContainer *body);
StmtNodeContainer *new_for_node(ExpNode *initialization, ExpNode *condition, 
    ExpNode *loop_expression, StmtNodeContainer *body);
StmtNodeContainer *new_return_node(ExpNode *expr);
StmtNodeContainer *new_assignment_node(ExpNode *expr);
StmtNodeContainer *new_invocation_node(ExpNode *expr);
StmtNodeContainer *new_block_node(Array *stmts);

Array *stmt_list_new(StmtNodeContainer *stmt);
void stmt_list_insert(Array *stmts, StmtNodeContainer *stmt);
#endif
