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

void insert_fn_prot(char *id, Array *tx) {
    Symbol * sym = malloc(sizeof(Symbol));
    sym->type = TYPE_FN_PROT;
    sym->is_array = 0;
    sym->return_type = current_type;
    sym->type_list = tx;
    _add_to_scope(id, sym);
}

void type_list_insert(Array *tx) {
    printf("Inserting type %d into type list\n", current_type);
    Array_append(tx, &current_type);
}

Array *type_list_new() {
    Array *tx = Array_init(4, sizeof(enum SymType));
    type_list_insert(tx);
    return tx;
}

void id_list_insert(Array *idx, char *id) {
    Array_append(idx, &id);
    assert(*((char **) Array_get(idx, idx->length - 1)) == id);
}

Array *id_list_new(char *id) {
    Array *idx = Array_init(0, sizeof(char *));
    id_list_insert(idx, id);
    return idx;
}

void dcl_map_insert(Env *dcl_map, Array *idx) {
    int i;
    for (i = 0; i < idx->length; i++) {
        char *id = *((char **) Array_get(idx, i));
        Symbol *sym = malloc(sizeof(Symbol));
        sym->type = current_type;
        printf("Inserting symbol %s of type %d into dcl_map\n", id, sym->type);
        if (!Env_put(current_scope, id, sym))
            fprintf(stderr, "Line %d: Ignoring duplicate declaration of identifier %s.\n", line_num, id);
    }
}

Env *dcl_map_new() {
    return Env_new(NULL);
}

/* 
 * Given a function idenifier, id_list, and declaration list:
 *   if a prototype exists
 *     see that the id list matches the prototype size
 *     create (id => sym map) m1
 *     verify that the m1 == declaration list
 *   else
 *     verify every id in declaration list is in the id_list
 *     verify every id in id_list is in declaration list
 *     construct type list
 * 
 *   create symbol entry for fn
 *   add fn to global symbol table
 *   add all symbols in declaration list to local symbol table
 *   
 */
void verify_fn_dcl(char *fn_id, Array *idx, Array *dclx) {
}
