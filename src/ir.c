#include <stdio.h>
#include <string.h>
#include "array.h"
#include "map.h"
#include "env.h"
#include "procedure.h"
#include "ir.h"
#include "str_table.h"
#include "instruction.h"
#include "statements.h"
#include "ir_statement.h"
#define TMP_VAR_NAME_BUFFER_SIZE 12
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
        printf("    ");
        switch(node->type) {
            case ARITHMETIC_INST: {
                ArithmeticInstruction *inst = node->value;
                char *op_str;
                switch (inst->op) {
                    case OP_ADD:
                        op_str = "+";
                        break;
                    case OP_SUB:
                    case OP_NEG:
                        op_str = "-";
                        break;
                    case OP_MUL:
                        op_str = "*";
                        break;
                    case OP_DIV:
                        op_str = "/";
                        break;
                }
                if (inst->rhs)
                    printf("%s = %s %s %s\n",
                        inst->return_symbol,
                        inst->lhs,
                        op_str,
                        inst->rhs
                    );
                else
                    printf("%s = %s %s\n",
                        inst->return_symbol,
                        op_str,
                        inst->lhs
                    );
                break;
            } case COPY_INST: {
                CopyInstruction *inst = node->value;
                if (inst->index)
                    printf("%s[%s] = %s\n",
                        inst->lhs,
                        inst->index,
                        inst->rhs
                    );
                else
                    printf("%s = %s\n",
                        inst->lhs,
                        inst->rhs
                    );
                break;
            } case COND_JUMP_INST: {
                ConditionalJumpInstruction *inst = node->value;
                char *dest = ((LabelInstruction *) inst->destination->value)->name;
                printf("if %s != 0 goto %s\n", inst->condition, dest);
                break;
            } case COND_COMP_JUMP_INST: {
                ConditionalComparisonJumpInstruction *inst = node->value;
                char *dest = ((LabelInstruction *) inst->destination->value)->name;
                char *op_str;
                switch (inst->op) {
                    case OP_EQ:
                        op_str = "==";
                        break;
                    case OP_NEQ:
                        op_str = "!=";
                        break;
                    case OP_GT:
                        op_str = ">";
                        break;
                    case OP_GE:
                        op_str = ">=";
                        break;
                    case OP_LT:
                        op_str = "<";
                        break;
                    case OP_LE:
                        op_str = "<=";
                        break;
                }
                printf("if %s %s %s goto %s\n", inst->lhs, op_str, inst->rhs, dest);
                break;
            } case UNCOND_JUMP_INST: {
                UnconditionalJumpInstruction *inst = node->value;
                char *dest = ((LabelInstruction *) inst->destination->value)->name;
                printf("goto %s\n", dest);
                break;
            } case INVOC_INST: {
                InvocationInstruction *inst = node->value;
                if (inst->return_symbol)
                    printf("%s = ", inst->return_symbol);
                printf("%s(", inst->fn);
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
            } case RETURN_INST: {
                ReturnInstruction *inst = node->value;
                printf("return");
                if (inst->return_symbol)
                    printf(" %s", inst->return_symbol);
                printf("\n");
                break;
            } case ARRAY_EL_INST: {
                ArrayElementInstruction *inst = node->value;
                printf("%s = %s[%s]\n", inst->return_symbol, inst->arr, 
                    inst->index);
                break;
            }
        }
        node = node->next;
    }
}

Instruction *procedure_to_ir(char *id, Env *env, Array *stmts) {
    Instruction *result = NULL;
    int i;
    if (stmts)
        for (i = 0; i < stmts->length; i++) {
            StmtNodeContainer *stmt = *((StmtNodeContainer **) array_get(stmts, i));
            result = concat_inst(2, result, statement_to_ir(env, stmt));
        }
    return result;
}

void print_global_var(void *k, void **v, void *info) {
    Env *env = info;
    char *id = k;
    Symbol *sym = *((Symbol **) v);
    switch (sym->type) {
        case TYPE_CHAR:
        case TYPE_INT:
            printf("global i32 %s", k);
            break;
        case TYPE_FLOAT:
            printf("global double %s", k);
            break;
        default:
            break;
    }
    if (sym->is_array)
        printf("[%d]", sym->array_length);
    printf("\n");
}

void print_ir(Env *global_scope, Array *procedures) {
    map_apply(&global_scope->table, &print_global_var, global_scope);
    int i, j;
    for (i = 0; i < procedures->length; i++) {
        Procedure *proc = array_get(procedures, i);
        Symbol *sym = Env_get(global_scope, proc->id);
        switch (proc->return_type) {
            case VOID_TYPE:
                printf("void");
                break;
            case INT_32:
                printf("i32");
                break;
            case FLOAT_64:
                printf("double");
                break;
            default:
                break;
        }
        printf(" %s(", proc->id);
        for (j = 0; j < sym->type_list->length; j++) {
            switch (*((int *) array_get(sym->type_list, j))) {
                case TYPE_CHAR:
                case TYPE_INT:
                    printf("i32");
                    break;
                case TYPE_FLOAT:
                    printf("double *");
                    break;
            }
            //printf(" %p, ", sym->param_list);
            printf(" %s", *((char **) array_get(sym->param_list, j)));
            if (j < sym->type_list->length - 1)
                printf(", ");
        }
        printf(") {\n");
        print_ir_list(proc->code);
        printf("}\n");
    }
}

void make_ir(Env *global_scope, Array *procedures) {
    int i;
    for (i = 0; i < procedures->length; i++) {
        Procedure *proc = array_get(procedures, i);
        Symbol *sym = Env_get(global_scope, proc->id);
        switch (sym->return_type) {
            case TYPE_VOID:
                proc->return_type = VOID_TYPE;
                break;
            case TYPE_CHAR:
            case TYPE_INT:
                proc->return_type = INT_32;
                break;
            case TYPE_FLOAT:
                proc->return_type = FLOAT_64;
                break;
            defaut:
                break;
        }
        proc->code = procedure_to_ir(proc->id, proc->env, proc->stmts);
    }
}
