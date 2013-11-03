#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "env.h"
extern int current_type;
extern Env *current_scope; 
extern int line_num;

void insert_symbol(char *id) {
    assert(current_scope);
    Symbol * sym = malloc(sizeof(Symbol));   
    sym->type = current_type;
    if (!Env_put(current_scope, id, sym))
        fprintf(stderr, "Line %d: Ignoring duplicate declaration of identifier %s.", line_num, id);
}

void verify_array(char *id) {
}
