#include "env.h"
#include "array.h"
#include "procedure.h"
void procedure_init(Procedure *fn, char *id, Env *env, Array *stmts) {
    fn->id = id;
    fn->env = env;
    fn->stmts = stmts;
    fn->code = NULL;
}
