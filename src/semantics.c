#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "env.h"
#include "array.h"
extern int current_type;
extern Env *current_scope; 
extern int line_num;

void _add_to_scope(char *id, Symbol * sym) {
    assert(current_scope);
    printf("Inserting symbol %s of type %d\n", id, sym->type);
    if (!Env_put(current_scope, id, sym))
        fprintf(stderr, "Line %d: Ignoring duplicate declaration of identifier %s.\n", line_num, id);
}

void insert_symbol(char *id) {
    Symbol * sym = malloc(sizeof(Symbol));   
    sym->type = current_type;
    sym->is_array = 0;
    _add_to_scope(id, sym);
}

void insert_array_symbol(char *id, int size) {
    Symbol * sym = malloc(sizeof(Symbol));
    sym->type = current_type; // + "*"
    sym->is_array = 1;
    _add_to_scope(id, sym);
}

void type_list_insert(Array *tx) {
    printf("Inserting type %d into type list\n", current_type);
    Array_append(tx, &current_type);
}

Array *new_type_list() {
    Array *tx = Array_init(4, sizeof(enum SymType));
    type_list_insert(tx);
    return tx;
}

void insert_fn_prot(char *id, Array *tx) {
    Symbol * sym = malloc(sizeof(Symbol));
    sym->type = TYPE_FN_PROT;
    sym->is_array = 0;
    sym->return_type = current_type;
    sym->type_list = tx;
    _add_to_scope(id, sym);
}
