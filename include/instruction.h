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

typedef enum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_NEG
} arith_op;

typedef struct {
    char *return_symbol;
    char *lhs;
    char *rhs;
    arith_op op;
} ArithmeticInstruction;

typedef struct {
    char *arr;
    char *index;
    char *return_symbol;
} ArrayElementInstruction;

typedef struct {
    char *lhs;
    char *rhs;
    char *index;
} CopyInstruction;

typedef struct {
    char *name;
} LabelInstruction;

struct Instruction;

typedef enum {
    OP_EQ,
    OP_NEQ,
    OP_GT,
    OP_GE,
    OP_LT,
    OP_LE
} comp_op;

typedef struct {
    comp_op op;
    char *lhs;
    char *rhs;
    struct Instruction *destination;
} ConditionalComparisonJumpInstruction;

typedef struct {
    char *condition;
    struct Instruction *destination;
} ConditionalJumpInstruction;

typedef struct {
    struct Instruction *destination;
} UnconditionalJumpInstruction;

typedef struct {
    char *return_symbol;
    char *fn;
    Array *params;
} InvocationInstruction;

typedef struct {
    char *return_symbol;
    int val;
} LoadIntInstruction;

typedef struct {
    char *return_symbol;
    double val;
} LoadFloatInstruction;

typedef struct {
    char *return_symbol;
} ReturnInstruction;

typedef struct Instruction {
    enum {
        ARITHMETIC_INST,
        COPY_INST,
        COND_JUMP_INST,
        COND_COMP_JUMP_INST,
        UNCOND_JUMP_INST,
        INVOC_INST,
        LOAD_INT_INST,
        LOAD_FLOAT_INST,
        LABEL_INST,
        RETURN_INST,
        ARRAY_EL_INST
    } type;
    void *value;
    struct Instruction *next;
} Instruction;

Instruction *concat_inst(int count, ...);
Instruction *arithmetic_instruction_new(char *op, char *lhs, char *rhs, char *sym);
Instruction *copy_instruction_new(char *lhs, char *rhs, char *index);
Instruction *load_int_instruction_new(int n);
Instruction *load_float_instruction_new(double n);
Instruction *label_instruction_new(char *name);
Instruction *cond_jump_instruction_new(char *sym, Instruction *destination);
Instruction *cond_comp_jump_instruction_new(char *op, char *lhs, char *rhs, Instruction *destination);
Instruction *uncond_jump_instruction_new(Instruction *destination);
Instruction *invocation_instruction_new(char *result_sym, char *fn_name, Array *params);
Instruction *return_instruction_new(char *return_sym);
Instruction *array_element_instruction_new(char *arr, char *index, char *return_sym);
#endif
