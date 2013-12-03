#include <stdio.h>
#include <string.h>
#include "ir.h"
#include "instruction.h"
#include "expressions.h"
Instruction *expr_to_ir(Env *env, ExpNode *expr, char **result_sym) {
    if (!expr)
        return NULL;
    printf("NODE TYPE : %d\n", expr->node_type);
    switch (expr->node_type) {
        case CONSTANT_EXPNODE: { // leaf
            printf("CONST EXPR!\n");
            Instruction *inst;
            if (expr->return_type == TYPE_INT)
                inst = load_int_instruction_new(expr->int_val);
            else
                inst = load_float_instruction_new(expr->float_val);

            char **ret_sym = &((LoadIntInstruction *) inst->value)->return_symbol;
            *ret_sym = next_tmp_symbol(env);
            if (result_sym)
                *result_sym = *ret_sym;
            //printf("Printing const expnode\n");
            //print_ir_list(inst);
            return inst;
            break;
        } case BOOLEAN_EXPNODE: {
            printf("Entering boolean expnode\n");
            // truth_val = 0
            // evaluate shit, jump to true if true
            // goto end
            // true:
            //   truth_val = 1
            // end:

            char *truth_var = next_tmp_symbol(env);

            Instruction *init = load_int_instruction_new(0);
            ((LoadIntInstruction *) init->value)->return_symbol = truth_var;

            Instruction *true_label = label_instruction_new(next_tmp_symbol(env));
            Instruction *end_label = label_instruction_new(next_tmp_symbol(env));

            char *lhs_sym, *rhs_sym;
            Instruction *lhs = expr_to_ir(env, expr->lhs, &lhs_sym); 
            Instruction *rhs = NULL;
            if (expr->rhs)
                rhs = expr_to_ir(env, expr->rhs, &rhs_sym); 

            Instruction *evaluate;
            printf("Before evaluate\n");
            if (!strcmp(expr->op, "||")) {
                // do first
                // if first, jump to true
                // do second
                // if second, jump to true
                Instruction *jump1 = cond_jump_instruction_new(lhs_sym, true_label);
                Instruction *jump2 = cond_jump_instruction_new(rhs_sym, true_label);
                evaluate = concat_inst(4, lhs, jump1, rhs, jump2);
            } else if (!strcmp(expr->op, "&&")) {
                // do first
                // if first, jump to maybe
                // goto end

                // maybe:
                // do second
                // if second, jump to true
                // (goto end)
                Instruction *maybe_label = label_instruction_new(next_tmp_symbol(env));
                Instruction *jump_to_maybe = cond_jump_instruction_new(lhs_sym, maybe_label);
                Instruction *jump_to_end = uncond_jump_instruction_new(end_label);
                Instruction *jump_to_true = cond_jump_instruction_new(rhs_sym, true_label);

                evaluate = concat_inst(6, lhs, jump_to_maybe, jump_to_end, 
                    maybe_label, rhs, jump_to_true);
            } else {
                printf("NEGATION\n");
                // (boolean negation)
                // do first
                // if first, jump to end
                // jump to true
                Instruction *jump_to_end = cond_jump_instruction_new(lhs_sym, end_label);
                Instruction *jump_to_true = uncond_jump_instruction_new(true_label);
                printf("CONCAT %p %p %p\n", lhs, jump_to_end, jump_to_true);
                evaluate = concat_inst(3, lhs, jump_to_end, jump_to_true);
            }
            printf("After evaluate\n");

            Instruction *goto_end = uncond_jump_instruction_new(end_label);

            char *load_one_var = next_tmp_symbol(env);
            Instruction *load_one = load_int_instruction_new(1);
            ((LoadIntInstruction *) load_one->value)->return_symbol = load_one_var;
            Instruction *set_true = copy_instruction_new(truth_var, load_one_var);

            if (result_sym)
                *result_sym = truth_var;
            // init
            // evaluate
            // goto end
            // true:
            // set_to_1
            // end:

            printf("Entering boolean expnode\n");
            return concat_inst(
                7,
                init,
                evaluate,
                goto_end,
                true_label,
                load_one,
                set_true,
                end_label
            );
            break;    
        } case COMP_EXPNODE: {
            // return value should be 0 or 1
            printf("Entering comp expnode\n");
            char *return_symbol = next_tmp_symbol(env);
            if (result_sym)
                *result_sym = return_symbol;

            char *lhs_sym, *rhs_sym;
            Instruction *lhs = expr_to_ir(env, expr->lhs, &lhs_sym);
            Instruction *rhs = expr_to_ir(env, expr->rhs, &rhs_sym);

            Instruction *inst_cont = arithmetic_instruction_new();
            ArithmeticInstruction *inst = inst_cont->value;
            inst->return_symbol = return_symbol;
            inst->lhs = lhs_sym;
            inst->rhs = rhs_sym;
            inst->op = expr->op;

            printf("Exiting comp expnode\n");
            return concat_inst(3, lhs, rhs, inst_cont);
            break;
        } case ARITHMETIC_EXPNODE: {
            printf("Entering math expnode\n");
            Instruction *inst_cont = arithmetic_instruction_new();
            ArithmeticInstruction *inst = inst_cont->value;

            inst->return_symbol = next_tmp_symbol(env);
            inst->op = expr->op;

            char *lhs_sym, *rhs_sym;
            Instruction *lhs = expr_to_ir(env, expr->lhs, &lhs_sym);
            Instruction *rhs = NULL;
            inst->lhs = lhs_sym;
            if (expr->rhs) {
                rhs = expr_to_ir(env, expr->rhs, &rhs_sym);
                inst->rhs = rhs_sym;
            }

            if (result_sym)
                *result_sym = inst->return_symbol;
            // prepend lhs and rhs and inst_cont
            //printf("%p %p %p\n", lhs, rhs, inst_cont);
            Instruction *result = concat_inst(3, lhs, rhs, inst_cont);
            //print_ir_list(result);
            printf("Exiting math expnode\n");
            return result;
            break;
        } case INVOCATION_EXPNODE: {
            Array *sym_list = array_new(0, sizeof(char **));
            Instruction *param_eval = NULL;
            int i;
            printf("count: %d\n", expr->expns->length);
            for (i = 0; i < expr->expns->length; i++) {
                char *sym_name;
                ExpNode *exp = *((ExpNode **) array_get(expr->expns, i));
                param_eval = concat_inst(2, param_eval, expr_to_ir(env, exp, &sym_name));
                printf("INVOC: %s\n", sym_name);
                array_append(sym_list, &sym_name);
            }

            char *return_symbol = next_tmp_symbol(env);
            Instruction *invocation = invocation_instruction_new(return_symbol, expr->op, sym_list);
            if (result_sym)
                *result_sym = return_symbol;

            return concat_inst(2, param_eval, invocation);
            break;
        } case ID_EXPNODE: // leaf
            printf("ID_EXPNODE: %s\n", expr->op);
            if (result_sym)
                *result_sym = expr->op;
            return NULL;
            break;
        case ASSIGNMENT_EXPNODE: {
            printf("Entering assignment expnode\n");
            char *lhs_sym, *rhs_sym;
            lhs_sym = rhs_sym = NULL;
            Instruction *lhs = expr_to_ir(env, expr->lhs, &lhs_sym);
            Instruction *rhs = expr_to_ir(env, expr->rhs, &rhs_sym);
            printf("LHS: %p, RHS: %p\n", lhs_sym, rhs_sym);
            Instruction *cpy_inst = copy_instruction_new(lhs_sym, rhs_sym);
            printf("Printing assignment expnode\n");
            Instruction *result = concat_inst(3, lhs, rhs, cpy_inst);
            //print_ir_list(result);
            return result;
            break;
        } default:
            break;
    }
    return NULL;
}

