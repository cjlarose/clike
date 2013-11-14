#ifndef _PROCEDURE_H_
#define _PROCEDURE_H_
#include "env.h"
#include "array.h"
typedef struct Procedure {
    Env *env;
    Array *stmts;
} Procedure;

void procedure_init(Procedure *fn, Env *env, Array *stmts);
#endif
