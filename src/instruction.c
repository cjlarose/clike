#include "instruction.h"

Instruction *concat_inst(Instruction *inst1, ...) {
    /*
    Instruction *node = lhs;
    while (node != NULL)
        node = node->next;
    node->next = rhs;
    */
    return NULL;
}

Instruction *_instruction_new(int type, void *value) {
    Instruction *inst_cont = malloc(sizeof(Instruction));
    inst_cont->type = type;
    inst_cont->value = value;
    inst_cont->next = NULL;
    return inst_cont;
}

Instruction *arithmetic_instruction_new() {
    return _instruction_new(ARITHMETIC_INST, 
        calloc(1, sizeof(ArithmeticInstruction)));
}

Instruction *copy_instruction_new(char *lhs, char *rhs) {
    CopyInstruction *cpy_inst = malloc(sizeof(CopyInstruction));
    cpy_inst->lhs = lhs;
    cpy_inst->rhs = rhs;
    return _instruction_new(COPY_INST, cpy_inst);
}

Instruction *load_int_instruction_new(int n) {
    LoadIntInstruction *inst = malloc(sizeof(LoadIntInstruction));
    inst->val = n;
    return _instruction_new(LOAD_INT_INST, inst);
}

Instruction *load_float_instruction_new(double n) {
    LoadIntInstruction *inst = malloc(sizeof(LoadIntInstruction));
    inst->val = n;
    return _instruction_new(LOAD_FLOAT_INST, inst);
}

Instruction *label_instruction_new(char *name) {
    LabelInstruction *inst = malloc(sizeof(LabelInstruction));
    inst->name = name;
    return _instruction_new(LABEL_INST, inst);
}

Instruction *cond_jump_instruction_new(char *sym, Instruction *destination) {
    return NULL;
}
Instruction *uncond_jump_instruction_new(Instruction *destination) {
    return NULL;
}
