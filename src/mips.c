#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "array.h"
#include "env.h"
#include "procedure.h"
#include "instruction.h"
#include "map.h"

void print_data(void *k, void **v, void *info) {
    Env *env = info;
    char *id = k;
    Symbol *sym = *((Symbol **) v);
    int i;
    int length = 1;
    if (sym->is_array)
        length = sym->array_length;

    switch (sym->type) {
        case TYPE_CHAR:
        case TYPE_INT:
            printf("%s:", id);
            for (i = 0; i < length; i++)
                printf(" .word");
            printf("\n");
            break;
        case TYPE_FLOAT:
            printf("%s:", id);
            for (i = 0; i < length; i++)
                printf(" .double");
            printf("\n");
            break;
        default:
            break;
    }
}

void print_globals(Env *scope) {
    printf(".data\n\n");
    map_apply(&scope->table, &print_data, scope);
}

void print_inst(char *inst, char *format_str, ...) {
    printf("    %-8s", inst);
    va_list ap;
    va_start(ap, format_str);
    vfprintf(stdout, format_str, ap);
    va_end(ap); 
    printf("\n");
}

int get_frame_size(int local_storage, int num_args) {
    int size = 8; // just to store $ra and %fp
    size += local_storage;
    size += 4 * num_args;

    // sweet, now align to double word boundary
    if (size % 8 != 0)
        size += 8 - (size % 8);
    return size;
}

void print_prologue(Procedure *proc, int num_args, int frame_size) {
    print_inst("subu", "$sp, $sp, %d", frame_size);
    print_inst("sw", "$ra, %d($sp)", 4 * num_args + 4);
    print_inst("sw", "$fp, %d($sp)", 4 * num_args);
    print_inst("addiu", "$fp, $sp, %d", frame_size - 4);
}

void print_epilogue(Procedure *proc, int num_args, int frame_size) {
    print_inst("lw", "$ra, %d($sp)", 4 * num_args + 4);
    print_inst("lw", "$fp, %d($sp)", 4 * num_args);
    print_inst("addiu", "$sp, $sp, %d", frame_size);
    print_inst("jr", "$ra");
}

void allocate_local(void *k, void **v, void *data) {
    int *frame_size = data;
    Symbol *sym = *((Symbol **) v);
    int size = 0;
    switch (sym->type) {
        case TYPE_CHAR:
        case TYPE_INT:
        case TYPE_BOOL:
            size = 4;
            break;
        case TYPE_FLOAT:
            size = 8;
            break;
        default:
            break;
    }

    if (*frame_size % size != 0)
        *frame_size += size - (*frame_size % size);

    sym->offset = *frame_size;
    sym->size = size;

    *frame_size += size;
}

void print_size(void *k, void **v, void *info) {
    char *id = k;
    int *size = *((int **) v);
    printf("%s => %d\n", id, *size);
}

int allocate_locals(Procedure *proc) {
    int size = 0;
    map_apply(&(proc->env->table), &allocate_local, &size);
    return size;
}

void load_word(Procedure *proc, char *dest, char *var) {
    Symbol **sym;
    if ((sym = (Symbol**) map_find(&proc->env->table, var))) {
        print_inst("lw", "%s, %d($fp)", dest, (*sym)->offset);
    } else {
        print_inst("la", "%s, %s", dest, var);
        print_inst("lw", "%s, %s", dest, dest);
    }
}

void store_word(Procedure *proc, char *src, char *var) {
    Symbol **sym;
    if ((sym = (Symbol**) map_find(&proc->env->table, var))) {
        print_inst("sw", "%s, %d($fp)", src, (*sym)->offset);
    } else {
        print_inst("la", "$t0, %s", var);
        print_inst("sw", "%s, $t0", src);
    }
}

void store_double(Procedure *proc, char *src, char *var) {
    // TODO: this
    printf("# LOL IDK HOW TO DO THAT\n");
}

void print_inst_node(Procedure *proc, Instruction *node) {
    switch (node->type) {
        case ARITHMETIC_INST: {
            // -x / * + - 
            ArithmeticInstruction *inst = node->value;

            char *op_str;
            switch (inst->op) {
                case OP_ADD:
                    op_str = "add";
                    break;
                case OP_SUB:
                    op_str = "sub";
                    break;
                case OP_MUL:
                    op_str = "mul";
                    break;
                case OP_DIV:
                    op_str = "div";
                    break;
                case OP_NEG:
                    op_str = "neg";
                    break;
            }

            load_word(proc, "$t0", inst->lhs);
            if (inst->rhs) {
                load_word(proc, "$t1", inst->rhs);
                print_inst(op_str, "$t2, $t0, $t1");
            } else
                print_inst(op_str, "$t2, $t0");
            store_word(proc, "$t2", inst->return_symbol);
            break;
        } case COPY_INST: {
            // TODO: inst->index
            CopyInstruction *inst = node->value;
            load_word(proc, "$t0", inst->lhs);
            store_word(proc, "$t0", inst->rhs);
            break;
        } case COND_JUMP_INST: {
            ConditionalJumpInstruction *inst = node->value;
            char *dest = ((LabelInstruction *) inst->destination->value)->name;
            load_word(proc, "$t0", inst->condition);
            print_inst("bne", "$t0, $zero, %s", dest);
            break;
        } case COND_COMP_JUMP_INST: {
            ConditionalComparisonJumpInstruction *inst = node->value;
            char *dest = ((LabelInstruction *) inst->destination->value)->name;
            char *op_str;
            switch (inst->op) {
                case OP_EQ:
                    op_str = "beq";
                    break;
                case OP_NEQ:
                    op_str = "bne";
                    break;
                case OP_GT:
                    op_str = "bgt";
                    break;
                case OP_GE:
                    op_str = "bge";
                    break;
                case OP_LT:
                    op_str = "blt";
                    break;
                case OP_LE:
                    op_str = "ble";
                    break;
            }
            load_word(proc, "$t0", inst->lhs);
            load_word(proc, "$t1", inst->rhs);
            print_inst(op_str, "$t0, $t1, %s", dest);
            break;
        } case UNCOND_JUMP_INST: {
            UnconditionalJumpInstruction *inst = node->value;
            char *dest = ((LabelInstruction *) inst->destination->value)->name;
            print_inst("j", "%s", dest);
            break;
        } case LOAD_INT_INST: {
            LoadIntInstruction *inst = node->value;
            print_inst("addi", "$t0, $zero, %d", inst->val);
            store_word(proc, "$t0", inst->return_symbol);
            break;
        } case LOAD_FLOAT_INST: {
            LoadFloatInstruction *inst = node->value;
            print_inst("li.d", "$f0, %f", inst->val);
            store_double(proc, "$f0", inst->return_symbol);
            break;
        } case LABEL_INST: {
            LabelInstruction *inst = node->value;
            printf("    %s:\n", inst->name);
            break;
        } case RETURN_INST: {
            ReturnInstruction *inst = node->value;
            if (inst->return_symbol)
                load_word(proc, "$v0", inst->return_symbol);
            print_inst("j", "%s_epilogue", proc->id);
            break;
        } default:
            printf("%d\n", node->type);
            //assert(0);
            break;
    }
}

void print_procedure(Procedure *proc) {
    int num_args = 4; // TODO: get correct num args
    int local_size = allocate_locals(proc);
    int frame_size = get_frame_size(local_size, num_args);

    printf("%s:\n", proc->id);
    print_prologue(proc, num_args, frame_size);
    Instruction *inst = proc->code;
    for (; inst; inst = inst->next)
        print_inst_node(proc, inst);
    printf("    %s_epilogue:\n", proc->id); // ending label
    print_epilogue(proc, num_args, frame_size);
}

void print_mips(Env *global_scope, Array *procedures) {
    print_globals(global_scope);
    printf("\n");
    printf(".text\n\n");
    int i;
    for (i = 0; i < procedures->length; i++)
        print_procedure((Procedure *) array_get(procedures, i));
}
