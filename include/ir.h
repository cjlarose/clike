#ifndef _IR_H_
#define _IR_H_
#include "array.h"
#include "env.h"
char *next_tmp_var_name();
char *next_tmp_symbol(Env *env);
void print_ir(Env *global_scope, Array *procedures);
#endif
