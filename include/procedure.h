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
} Procedure;

void procedure_init(Procedure *fn, char *id, Env *env, Array *stmts);
#endif
