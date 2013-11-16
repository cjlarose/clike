#include <stdio.h>
#include "array.h"
#include "map.h"
#include "env.h"
#include "procedure.h"
#include "ir.h"
#include "statements.h"
#include "expressions.h"
#include "str_table.h"
#define TMP_VAR_NAME_BUFFER_SIZE 12
#define CONST_BUFFER_SIZE 64
extern StringTable str_table;

char *next_tmp_var_name() {
    static int num = 0;
    char buffer[TMP_VAR_NAME_BUFFER_SIZE];
    snprintf(buffer, TMP_VAR_NAME_BUFFER_SIZE, "t%d", num++);
    return str_table_get(&str_table, buffer);
}

// just populate var_names appropriately
void expr_to_ir(ExpNode *expr, Map *var_names) {
    //printf("EXPRESSION of type %d!\n", expr->node_type);
    char buffer[CONST_BUFFER_SIZE];
    switch (expr->node_type) {
        case CONSTANT_EXPNODE: // leaf
            //printf("CONST EXPR!\n");
            if (expr->return_type == TYPE_INT)
                snprintf(buffer, CONST_BUFFER_SIZE, "%d", expr->int_val);
            else
                snprintf(buffer, CONST_BUFFER_SIZE, "%f", expr->float_val);
            map_insert(var_names, expr, str_table_get(&str_table, buffer));
            break;
        case BOOLEAN_EXPNODE:
            //printf("BOOL EXPR!\n");
            break;
        case COMP_EXPNODE:
            //printf("COMP EXPR!\n");
            break;
        case ARITHMETIC_EXPNODE:
            //printf("MATH EXPR!\n");
            expr_to_ir(expr->lhs, var_names);
            if (expr->rhs)
                expr_to_ir(expr->rhs, var_names);
            map_insert(var_names, expr, next_tmp_var_name());
            break;
        case INVOCATION_EXPNODE:
            //printf("INVOCATION EXPR!\n");
            break;
        case ID_EXPNODE: // leaf
            //printf("ID EXPR!\n");
            map_insert(var_names, expr, expr->op);
            break;
        case ASSIGNMENT_EXPNODE:
            //printf("ASSIGNMENT EXPR!\n");
            expr_to_ir(expr->lhs, var_names);
            expr_to_ir(expr->rhs, var_names);
            break;
        default:
            break;
    }
}

void print_map(void *k, void **v, void *_) {
    printf("Expr %p => Var %s\n", k, *((char **) v));
}

void print_expr_ir(ExpNode *expr, Map *var_names) {
    //map_apply(var_names, &print_map, NULL);
    switch (expr->node_type) {
        case ASSIGNMENT_EXPNODE:
            print_expr_ir(expr->lhs, var_names);
            print_expr_ir(expr->rhs, var_names);
            printf("%s = %s\n", *((char **) map_find(var_names, expr->lhs)), 
                *((char **) map_find(var_names, expr->rhs)));
            break;
        case ARITHMETIC_EXPNODE:
            print_expr_ir(expr->lhs, var_names);
            if (expr->rhs) { 
                print_expr_ir(expr->rhs, var_names);
                printf("%s = %s %s %s\n", 
                    *((char **) map_find(var_names, expr)), 
                    *((char **) map_find(var_names, expr->lhs)), 
                    expr->op, 
                    *((char **) map_find(var_names, expr->rhs)));
            } else 
                printf("%s = %s %s\n", 
                    *((char **) map_find(var_names, expr)), 
                    expr->op, 
                    *((char **) map_find(var_names, expr->lhs)));
            break;
        default:
            break;
    }
}

void assg_stmt_to_ir(AssignmentStatement *stmt, Map *var_names) {
    //printf("ASSIGNMENT STMT!\n");
    expr_to_ir(stmt->expr, var_names);
    print_expr_ir(stmt->expr, var_names);
}

void statement_to_ir(StmtNodeContainer *stmt, Map *var_names) {
    printf("PRINTING IR FOR STMT %p of type %d\n", stmt, stmt->type);
    switch (stmt->type) {
        case ASSIGNMENT_STMT:
            assg_stmt_to_ir(&stmt->node.assg_stmt, var_names);
            break;
        default:
            break;
    }
}

void print_ir_procedure(Env *env, Array *stmts) {
    printf("PRINTING IR FOR ENV %p\n", env);
    Map var_names;// expnode *=> var name
    map_init(&var_names, NULL, NULL, 4);
    int i;
    if (stmts)
        for (i = 0; i < stmts->length; i++) {
            StmtNodeContainer *stmt = *((StmtNodeContainer **) array_get(stmts, i));
            statement_to_ir(stmt, &var_names);
        }
}

void print_ir(Env *global_scope, Array *procedures) {
    printf("PRINTING INTERMEDIATE CODE\n");
    int i;
    for (i = 0; i < procedures->length; i++) {
        Procedure *proc = array_get(procedures, i);
        print_ir_procedure(proc->env, proc->stmts);
    }
}
