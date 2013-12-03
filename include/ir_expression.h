#ifndef _IR_EXPRESSION_H
#define _IR_EXPRESSION_H_
#include "env.h"
#include "instruction.h"
#include "expressions.h"
Instruction *expr_to_ir(Env *env, ExpNode *expr, char **result_sym);
#endif
