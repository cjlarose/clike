#ifndef _IR_STATEMENT_H_
#define _IR_STATEMENT_H_
#include "instruction.h"
#include "env.h"
#include "statements.h"
Instruction *statement_to_ir(Env *env, StmtNodeContainer *stmt);
#endif
