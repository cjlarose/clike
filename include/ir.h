#ifndef _IM_H_
#define _IM_H_
#include "array.h"
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
    char *condition;
    char *destination;
} JumpInstruction;

typedef struct {
    char *fn;
    Array params;
} InvocationInstruction;

typedef struct Instruction {
    enum {
        ARITHMETIC_INST,
        COPY_INST,
        JUMP_INST,
        INVOC_INST
    } type;
    void *value;
    struct Instruction *next;
} Instruction;

void print_ir(Env *global_scope, Array *procedures);
#endif
