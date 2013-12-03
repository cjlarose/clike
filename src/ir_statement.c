#include <stdio.h>
#include "instruction.h"
#include "ir.h"
#include "env.h"
#include "statements.h"
#include "ir_statement.h"
#include "ir_expression.h"

Instruction *if_stmt_to_ir(Env *env, IfStatement *stmt) {
    char *condition_var = next_tmp_symbol(env);
    Instruction *condition = expr_to_ir(env, stmt->condition, &condition_var);
    Instruction *then_stmt = statement_to_ir(env, stmt->then_stmt);
    Instruction *else_stmt = NULL;
    if (stmt->else_stmt)
        else_stmt = statement_to_ir(env, stmt->else_stmt);

    Instruction *true_label = label_instruction_new(next_tmp_symbol(env));
    Instruction *end_label = label_instruction_new(next_tmp_symbol(env));

    Instruction *jump_to_true = cond_jump_instruction_new(condition_var, 
        true_label);
    Instruction *jump_to_end = uncond_jump_instruction_new(end_label);

    // for ifs with else stmts

    // condition
    // if (condition) goto true
    // else code
    // jump to end
    // true:
    // then code
    // end:

    /*
    printf("Condition: %p\n", condition);
    print_ir_list(condition);
    printf("Jump To True: %p\n", jump_to_true);
    print_ir_list(jump_to_true);
    printf("Else Stmt: %p\n", else_stmt);
    print_ir_list(else_stmt);
    printf("Jump To End: %p\n", jump_to_end);
    print_ir_list(jump_to_end);
    printf("True Label: %p\n", true_label);
    print_ir_list(true_label);
    printf("Then Stmt: %p\n", then_stmt);
    print_ir_list(then_stmt);
    printf("End Label: %p\n", end_label);
    print_ir_list(end_label);
    printf("End\n");
    */

    return concat_inst(
        7,
        condition,
        jump_to_true,
        else_stmt,
        jump_to_end,
        true_label,
        then_stmt,
        end_label
    );
}

Instruction *assg_stmt_to_ir(Env *env, AssignmentStatement *stmt) {
    //printf("ASSIGNMENT STMT!\n");
    //print_expr_ir(stmt->expr);
    Instruction *expr_inst = expr_to_ir(env, stmt->expr, NULL);
    return expr_inst;
}

Instruction *statement_to_ir(Env *env, StmtNodeContainer *stmt) {
    printf("PRINTING IR FOR STMT %p of type %d\n", stmt, stmt->type);
    switch (stmt->type) {
        case IF_STMT:
            return if_stmt_to_ir(env, &stmt->node.if_stmt);
            break;
        case ASSIGNMENT_STMT:
            return assg_stmt_to_ir(env, &stmt->node.assg_stmt);
            break;
        default:
            break;
    }
    return NULL;
}
