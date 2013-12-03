#include <stdio.h>
#include "instruction.h"
#include "ir.h"
#include "env.h"
#include "statements.h"
#include "ir_statement.h"
#include "ir_expression.h"

Instruction *boolean_eval(Env *env, ExpNode *condition_expr, 
    Instruction *then_stmt, Instruction *else_stmt) {

    char *condition_var = next_tmp_symbol(env);
    Instruction *condition = NULL;
    if (condition_expr)
        condition = expr_to_ir(env, condition_expr, &condition_var);

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

Instruction *if_stmt_to_ir(Env *env, IfStatement *stmt) {
    Instruction *then_stmt = statement_to_ir(env, stmt->then_stmt);
    Instruction *else_stmt = NULL;
    if (stmt->else_stmt)
        else_stmt = statement_to_ir(env, stmt->else_stmt);
    return boolean_eval(env, stmt->condition, then_stmt, else_stmt);
}

Instruction *while_eval(Env *env, ExpNode *condition, Instruction *loop_body) {
    // loop begin:
    // condition
    // if condition goto body: 
    // goto end
    // body:
    // do stuff
    // goto loop begin
    // end:

    Instruction *loop_begin = label_instruction_new(next_tmp_symbol(env));
    Instruction *goto_loop_begin = uncond_jump_instruction_new(loop_begin);

    Instruction *then_stmt = concat_inst(2, loop_body, goto_loop_begin);

    Instruction *end_label = label_instruction_new(next_tmp_symbol(env));
    Instruction *goto_end = uncond_jump_instruction_new(end_label);

    Instruction *main_body = boolean_eval(env, condition, then_stmt, 
        goto_end);
    return concat_inst(3, loop_begin, main_body, end_label);
}

Instruction *while_stmt_to_ir(Env *env, WhileStatement *stmt) {
    // expr non-null, body nullable
    Instruction *loop_body = statement_to_ir(env, stmt->body);
    return while_eval(env, stmt->condition, loop_body);
}

Instruction *for_stmt_to_ir(Env *env, ForStatement *stmt) {
    // all members can be null
    Instruction *init = NULL;
    if (stmt->initialization)
        init = expr_to_ir(env, stmt->initialization, NULL);

    return concat_inst(
        2,
        init,
        while_eval(env, stmt->condition, 
            concat_inst(2, stmt->body, stmt->loop_expression))
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
        case WHILE_STMT:
            return while_stmt_to_ir(env, &stmt->node.while_stmt);
            break;
        case FOR_STMT:
            return for_stmt_to_ir(env, &stmt->node.for_stmt);
            break;
        case ASSIGNMENT_STMT:
            return assg_stmt_to_ir(env, &stmt->node.assg_stmt);
            break;
        default:
            break;
    }
    return NULL;
}
