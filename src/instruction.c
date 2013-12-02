#include "instruction.h"

Instruction *concat_inst(Instruction *lhs, Instruction *rhs) {
    Instruction *node = lhs;
    while (node != NULL)
        node = node->next;
    node->next = rhs;
    return lhs;
}

Instruction *arithmetic_instruction_new() {
    Instruction *inst_cont = malloc(sizeof(Instruction));
    inst_cont->type = ARITHMETIC_INST;
    ArithmeticInstruction *inst = inst_cont->value = 
        calloc(1, sizeof(ArithmeticInstruction)); 
    inst_cont->next = NULL;
    return inst_cont;
}
