#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_
#include "env.h"

/*
math
    return_symbol
    lhs_symbol
    lhs_offset_symbol // nullable
    rhs_symbol //nullable
    rhs_offset_symbol // nullable
    op

copy
    lhs_symbol
    rhs_symbol

jump
    condition_symbol (with value 0 or 1) // nullable
    destination label // code block?

invocation
    fn_symbol
    params // list of symbols
*/

typedef struct {
    char *return_symbol;
    char *lhs;
    char *lhs_offset;
    char *rhs;
    char *rhs_offset;
    char *op;
} ArithmeticInstruction;

typedef struct {
    char *lhs;
    char *rhs;
} CopyInstruction;

typedef struct {
    char *name;
} LabelInstruction;

typedef struct {
    char *condition;
    LabelInstruction *destination;
} JumpInstruction;

typedef struct {
    char *fn;
    Array params;
} InvocationInstruction;

typedef struct {
    char *return_symbol;
    int val;
} LoadIntInstruction;

typedef struct {
    char *return_symbol;
    double val;
} LoadFloatInstruction;

typedef struct Instruction {
    enum {
        ARITHMETIC_INST,
        COPY_INST,
        JUMP_INST,
        INVOC_INST,
        LOAD_INT_INST,
        LOAD_FLOAT_INST,
        LABEL_INST
    } type;
    void *value;
    struct Instruction *next;
} Instruction;

Instruction *concat_inst(Instruction *lhs, Instruction *rhs);
Instruction *arithmetic_instruction_new();
Instruction *copy_instruction_new(char *lhs, char *rhs);
Instruction *load_int_instruction_new(int n);
Instruction *load_float_instruction_new(double n);
Instruction *label_instruction_new(char *name);
#endif
