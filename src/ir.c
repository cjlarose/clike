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

char *print_expr_ir(ExpNode *expr) {
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
            char *lhs_var = print_expr_ir(expr->lhs);
            if (expr->rhs) { 
                char *rhs_var = print_expr_ir(expr->rhs);
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
            char *lhs_var = print_expr_ir(expr->lhs);
            char *rhs_var = print_expr_ir(expr->rhs);
            printf("%s = %s\n", lhs_var, rhs_var);
            break;
        } default:
            break;
    }
    return NULL;
}

Instruction *expr_to_ir(Env *env, ExpNode *expr, char **result_sym) {
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
            return inst;
            break;
        } case BOOLEAN_EXPNODE: {
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
            Instruction *rhs = expr_to_ir(env, expr->rhs, &rhs_sym); 

            Instruction *evaluate;
            if (!strcmp(expr->op, "||")) {
                // do first
                // if first, jump to true
                // do second
                // if second, jump to true
                Instruction *jump1 = cond_jump_instruction_new(lhs_sym, true_label);
                Instruction *jump2 = cond_jump_instruction_new(rhs_sym, true_label);
                evaluate = concat_inst(lhs, jump1, rhs, jump2);
            } else {//if (!strcmp(expr->op, "&&")) {
                // do first
                // do second
                // preform &
                // if true, jump to true
                char *and_symbol;
                Instruction *and = arithmetic_instruction_new();
                ArithmeticInstruction *and_inst = and->value;
                and_inst->op = "&";
                and_inst->lhs = lhs_sym;
                and_inst->rhs = rhs_sym;
                and_inst->return_symbol = and_symbol = next_tmp_symbol(env);

                Instruction *jump = cond_jump_instruction_new(and_symbol, true_label);
                evaluate = concat_inst(lhs, rhs, and, jump);
            }

            Instruction *goto_end = uncond_jump_instruction_new(end_label);

            char *load_one_var = next_tmp_symbol(env);
            Instruction *load_one = load_int_instruction_new(1);
            ((LoadIntInstruction *) init->value)->return_symbol = load_one_var;
            Instruction *set_true = copy_instruction_new(truth_var, load_one_var);

            // init
            // evaluate
            // goto end
            // true:
            // set_to_1
            // end:
            return concat_inst(
                init,
                evaluate,
                goto_end,
                true_label,
                load_one_var,
                set_true,
                end_label
            );
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
            if (expr->rhs)
                rhs = expr_to_ir(env, expr->rhs, &rhs_sym);

            if (result_sym)
                *result_sym = inst->return_symbol;
            // prepend lhs and rhs and inst_cont
            inst->lhs = lhs_sym;
            inst->rhs = rhs_sym;
            return concat_inst(concat_inst(lhs, rhs), inst_cont);
            break;
        } case ID_EXPNODE: // leaf
            if (result_sym)
                *result_sym = expr->op;
            return NULL;
            break;
        case ASSIGNMENT_EXPNODE: {
            printf("Entering assignment expnode\n");
            char *lhs_sym, *rhs_sym;
            Instruction *lhs = expr_to_ir(env, expr->lhs, &lhs_sym);
            Instruction *rhs = expr_to_ir(env, expr->rhs, &rhs_sym);
            Instruction *cpy_inst = copy_instruction_new(lhs_sym, rhs_sym);
            return concat_inst(concat_inst(lhs, rhs), cpy_inst);
            break;
        } default:
            break;
    }
    return NULL;
}

void assg_stmt_to_ir(Env *env, AssignmentStatement *stmt) {
    //printf("ASSIGNMENT STMT!\n");
    //print_expr_ir(stmt->expr);
    expr_to_ir(env, stmt->expr, NULL);
}

void statement_to_ir(Env *env, StmtNodeContainer *stmt) {
    printf("PRINTING IR FOR STMT %p of type %d\n", stmt, stmt->type);
    switch (stmt->type) {
        case ASSIGNMENT_STMT:
            assg_stmt_to_ir(env, &stmt->node.assg_stmt);
            break;
        default:
            break;
    }
}

void print_ir_procedure(Env *env, Array *stmts) {
    printf("PRINTING IR FOR ENV %p\n", env);
    int i;
    if (stmts)
        for (i = 0; i < stmts->length; i++) {
            StmtNodeContainer *stmt = *((StmtNodeContainer **) array_get(stmts, i));
            statement_to_ir(env, stmt);
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
