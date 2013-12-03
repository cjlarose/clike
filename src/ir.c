#include <stdio.h>
#include <string.h>
#include "array.h"
#include "map.h"
#include "env.h"
#include "procedure.h"
#include "ir.h"
#include "statements.h"
#include "expressions.h"
#include "str_table.h"
#include "instruction.h"
#define TMP_VAR_NAME_BUFFER_SIZE 12
#define CONST_BUFFER_SIZE 64
extern StringTable str_table;

char *next_tmp_var_name() {
    static int num = 0;
    char buffer[TMP_VAR_NAME_BUFFER_SIZE];
    snprintf(buffer, TMP_VAR_NAME_BUFFER_SIZE, "t%d", num++);
    return str_table_get(&str_table, buffer);
}

char *next_tmp_symbol(Env *env) {
    char *name = next_tmp_var_name();
    Symbol *sym = calloc(1, sizeof(Symbol));
    sym->scope = NULL; 
    sym->type = TYPE_INT; // unused
    Env_put(env, name, sym);
    return name;
}

void print_map(void *k, void **v, void *_) {
    printf("Expr %p => Var %s\n", k, *((char **) v));
}

void print_ir_list(Instruction *node) {
    while (node) {
        switch(node->type) {
            case ARITHMETIC_INST: {
                ArithmeticInstruction *inst = node->value;
                printf("%s = %s %s %s\n",
                    inst->return_symbol,
                    inst->lhs,
                    inst->op,
                    inst->rhs
                );
                break;
            } case COPY_INST: {
                CopyInstruction *inst = node->value;
                printf("%s = %s\n",
                    inst->lhs,
                    inst->rhs
                );
                break;
            } case JUMP_INST: {
                JumpInstruction *inst = node->value;
                char *dest = ((LabelInstruction *) inst->destination->value)->name;
                if (inst->condition)
                    printf("if %s goto %s\n", inst->condition, dest);
                else
                    printf("goto %s\n", dest);
                break;
            } case INVOC_INST: {
                InvocationInstruction *inst = node->value;
                printf("%s = %s(", inst->return_symbol, inst->fn);
                int i;
                for (i = 0; i < inst->params->length; i++)
                    printf("%s, ", *((char **) array_get(inst->params, i)));
                printf(")\n");
                break;
            } case LOAD_INT_INST: {
                LoadIntInstruction *inst = node->value;
                printf("%s = %d\n", inst->return_symbol, inst->val);
                break;
            } case LOAD_FLOAT_INST: {
                LoadFloatInstruction *inst = node->value;
                printf("%s = %f\n", inst->return_symbol, inst->val);
                break;
            } case LABEL_INST: {
                LabelInstruction *inst = node->value;
                printf("%s:\n", inst->name);
                break;
            }
        }
        node = node->next;
    }
}

Instruction *expr_to_ir(Env *env, ExpNode *expr, char **result_sym) {
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

Instruction *statement_to_ir(Env *env, StmtNodeContainer *stmt);

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

void print_ir_procedure(Env *env, Array *stmts) {
    printf("PRINTING IR FOR ENV %p\n", env);
    int i;
    if (stmts)
        for (i = 0; i < stmts->length; i++) {
            StmtNodeContainer *stmt = *((StmtNodeContainer **) array_get(stmts, i));
            Instruction *result = statement_to_ir(env, stmt);
            if (result)
                print_ir_list(result);
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
