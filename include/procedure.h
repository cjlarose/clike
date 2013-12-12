#ifndef _PROCEDURE_H_
#define _PROCEDURE_H_
#include "env.h"
#include "array.h"
#include "instruction.h"
typedef struct Procedure {
    char *id;
    Env *env;
    Array *stmts;
    Instruction *code;
    enum {
        VOID_TYPE,
        INT_32,
        FLOAT_64
    } return_type;
} Procedure;

void procedure_init(Procedure *fn, char *id, Env *env, Array *stmts);
#endif
