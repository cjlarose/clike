#include "env.h"
#include "array.h"
#include "procedure.h"
void procedure_init(Procedure *fn, Env *env, Array *stmts) {
    fn->env = env;
    fn->stmts = stmts;
}
