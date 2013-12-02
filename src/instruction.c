#include "instruction.h"

Instruction *concat_inst(Instruction *lhs, Instruction *rhs) {
    /*
    Instruction *node = lhs;
    while (node != NULL)
        node = node->next;
    node->next = rhs;
    */
    return lhs;
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
