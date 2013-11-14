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

struct StatementNodeContainer;

typedef struct IfStatement {
    ExpNode *condition;
    struct StatementNodeContainer *if_stmt;
    struct StatementNodeContainer *else_stmt;
} IfStatement;

typedef struct WhileStatement {
    ExpNode *condition;
    struct StatementNodeContainer *body;
} WhileStatement;

typedef struct ForStatement {
    ExpNode *initialization;
    ExpNode *condition;
    ExpNode *loop_expression;
    struct StatementNodeContainer *body;
} ForStatement;

typedef struct ReturnStatement {
    ExpNode *value;
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

typedef struct StatementNodeContainer {
    enum StatementType type;
    union StatementNode node;
} StatementNodeContainer;
#endif
