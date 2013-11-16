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

void print_map(void *k, void **v, void *_) {
    printf("Expr %p => Var %s\n", k, *((char **) v));
}

char *print_expr_ir(ExpNode *expr, Map *var_names) {
    //map_apply(var_names, &print_map, NULL);
    char buffer[CONST_BUFFER_SIZE];
    switch (expr->node_type) {
        case CONSTANT_EXPNODE: // leaf
            //printf("CONST EXPR!\n");
            if (expr->return_type == TYPE_INT)
                snprintf(buffer, CONST_BUFFER_SIZE, "%d", expr->int_val);
            else
                snprintf(buffer, CONST_BUFFER_SIZE, "%f", expr->float_val);
            return str_table_get(&str_table, buffer);
            break;
        case ARITHMETIC_EXPNODE: {
            char *return_var = next_tmp_var_name();
            char *lhs_var = print_expr_ir(expr->lhs, var_names);
            if (expr->rhs) { 
                char *rhs_var = print_expr_ir(expr->rhs, var_names);
                printf("%s = %s %s %s\n", 
                    return_var,
                    lhs_var,
                    expr->op, 
                    rhs_var);
            } else 
                printf("%s = %s %s\n", 
                    return_var,
                    expr->op, 
                    lhs_var);
            return return_var;
            break;
        } case ID_EXPNODE: // leaf
            return expr->op;
            break;
        case ASSIGNMENT_EXPNODE: {
            char *lhs_var = print_expr_ir(expr->lhs, var_names);
            char *rhs_var = print_expr_ir(expr->rhs, var_names);
            printf("%s = %s\n", lhs_var, rhs_var);
            break;
        } default:
            break;
    }
    return NULL;
}

void assg_stmt_to_ir(AssignmentStatement *stmt, Map *var_names) {
    //printf("ASSIGNMENT STMT!\n");
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
