#include <stdio.h>
#include "array.h"
#include "env.h"
void print_globals(Env *scope) {
    printf("GLOBALS\n");
}

void print_mips(Env *global_scope, Array *procedures) {
    printf("MIPS\n");
    print_globals(global_scope);
}
