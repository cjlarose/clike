#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "instruction.h"

Instruction *concat_inst(int count, ...) {
    va_list inst_list;
    int i;
    Instruction *head, *current_node;
    head = current_node = NULL;

    va_start(inst_list, count);
    for (i = 0; i < count; i++) {
        Instruction *inst = va_arg(inst_list, Instruction *);
        if (!inst)
            continue;
        if (head == NULL)
            head = current_node = inst;
        else {
            while (current_node->next)
                current_node = current_node->next;
            current_node->next = inst;
        }
    }
    va_end(inst_list);
    return head;
}

Instruction *_instruction_new(int type, void *value) {
    Instruction *inst_cont = malloc(sizeof(Instruction));
    inst_cont->type = type;
    inst_cont->value = value;
    inst_cont->next = NULL;
    return inst_cont;
}

Instruction *arithmetic_instruction_new(char *op, char *lhs, char *rhs, char *sym) {
    ArithmeticInstruction *inst = calloc(1, sizeof(ArithmeticInstruction));
    inst->lhs = lhs;
    inst->rhs = rhs;
    inst->return_symbol = sym;

    if (strcmp(op, "+") == 0)
        inst->op = OP_ADD;
    else if (strcmp(op, "-") == 0) {
        if (rhs == NULL)
            inst->op = OP_NEG;
        else
            inst->op = OP_SUB;
    } else if (strcmp(op, "*") == 0)
        inst->op = OP_MUL;
    else /*if (strcmp(op, "/") == 0) {*/
        inst->op = OP_DIV;
    
    return _instruction_new(ARITHMETIC_INST, inst);
}

Instruction *copy_instruction_new(char *lhs, char *rhs, char *index) {
    CopyInstruction *cpy_inst = malloc(sizeof(CopyInstruction));
    cpy_inst->lhs = lhs;
    cpy_inst->rhs = rhs;
    cpy_inst->index = index;
    return _instruction_new(COPY_INST, cpy_inst);
}

Instruction *load_int_instruction_new(int n) {
    LoadIntInstruction *inst = malloc(sizeof(LoadIntInstruction));
    inst->val = n;
    return _instruction_new(LOAD_INT_INST, inst);
}

Instruction *load_float_instruction_new(double n) {
    LoadFloatInstruction *inst = malloc(sizeof(LoadFloatInstruction));
    inst->val = n;
    return _instruction_new(LOAD_FLOAT_INST, inst);
}

Instruction *label_instruction_new(char *name) {
    LabelInstruction *inst = malloc(sizeof(LabelInstruction));
    inst->name = name;
    return _instruction_new(LABEL_INST, inst);
}

Instruction *cond_jump_instruction_new(char *sym, Instruction *destination) {
    ConditionalJumpInstruction *inst = malloc(sizeof(ConditionalJumpInstruction));
    inst->condition = sym;
    inst->destination = destination;
    return _instruction_new(COND_JUMP_INST, inst);
}

Instruction *cond_comp_jump_instruction_new(char *op, char *lhs, char *rhs, 
    Instruction *destination) {
    ConditionalComparisonJumpInstruction *inst = 
        malloc(sizeof(ConditionalComparisonJumpInstruction));

    if (strcmp(op, "==") == 0)
        inst->op = OP_EQ;
    else if (strcmp(op, "!=") == 0)
        inst->op = OP_NEQ;
    else if (strcmp(op, ">") == 0)
        inst->op = OP_GT;
    else if (strcmp(op, ">=") == 0)
        inst->op = OP_GE;
    else if (strcmp(op, "<") == 0)
        inst->op = OP_LT;
    else /*if (strcmp(op, "<=") == 0)*/
        inst->op = OP_LE;

    inst->lhs = lhs;
    inst->rhs = rhs;
    inst->destination = destination;
    return _instruction_new(COND_COMP_JUMP_INST, inst);
}

Instruction *uncond_jump_instruction_new(Instruction *destination) {
    UnconditionalJumpInstruction *inst = malloc(sizeof(UnconditionalJumpInstruction));
    inst->destination = destination;
    return _instruction_new(UNCOND_JUMP_INST, inst);
}

Instruction *invocation_instruction_new(char *result_symbol, char *fn_name, Array *params) {
    InvocationInstruction *inst = malloc(sizeof(InvocationInstruction));
    inst->return_symbol = result_symbol;
    inst->fn = fn_name;
    inst->params = params;
    return _instruction_new(INVOC_INST, inst);
}

Instruction *return_instruction_new(char *return_symbol) {
    ReturnInstruction *inst = malloc(sizeof(ReturnInstruction));
    inst->return_symbol = return_symbol;
    return _instruction_new(RETURN_INST, inst);
}

Instruction *array_element_instruction_new(char *arr, char *index, char *return_symbol) {
    ArrayElementInstruction *inst = malloc(sizeof(ArrayElementInstruction));
    inst->arr = arr;
    inst->index = index;
    inst->return_symbol = return_symbol;
    return _instruction_new(ARRAY_EL_INST, inst);
}
