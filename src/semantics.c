#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include "env.h"
#include "array.h"
extern int current_type;
extern int current_return_type;
extern Env *current_scope; 
extern int line_num;
extern int status;

void print_error(char *message, ...) {
    va_list ap;
    va_start(ap, message);
    fprintf(stderr, "Line %d: Warning: ", line_num);
    vfprintf(stderr, message, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    status = 2;
}

void _add_to_scope(char *id, Symbol * sym) {
    assert(current_scope);
    //printf("Inserting symbol %s of type %d\n", id, sym->type);
    if (!Env_put(current_scope, id, sym))
        print_error("Ignoring duplicate declaration of identifier "
        "%s.", id);
}

void insert_symbol(char *id) {
    Symbol * sym = malloc(sizeof(Symbol));   
    sym->type = current_type;
    sym->is_array = 0;
    sym->type_list = NULL;
    _add_to_scope(id, sym);
}

void insert_array_symbol(char *id, int size) {
    Symbol * sym = malloc(sizeof(Symbol));
    sym->type = current_type; // + "*"
    sym->is_array = 1;
    sym->type_list = NULL;
    _add_to_scope(id, sym);
}

void insert_fn_prot(char *id, Array *tx) {
    Symbol * sym = malloc(sizeof(Symbol));
    sym->type = TYPE_FN_PROT;
    sym->is_array = 0;
    sym->return_type = current_return_type;
    sym->type_list = tx ? tx : Array_init(0, sizeof(enum SymType)); // hack
    _add_to_scope(id, sym);
}

void type_list_insert(Array *tx) {
    //printf("Inserting type %d into type list\n", current_type);
    Array_append(tx, &current_type);
}

Array *type_list_new() {
    Array *tx = Array_init(0, sizeof(enum SymType));
    type_list_insert(tx);
    return tx;
}

void id_list_insert(Array *idx, char *id) {
    // stored a stack variable in array?
    //char ** ptr = malloc(sizeof(char **));
    //*ptr = id;
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
        //printf("idx[%d] = %p\n", i, Array_get(idx, i));
        Symbol *sym = malloc(sizeof(Symbol));
        sym->type_list = NULL;
        sym->type = current_type;
        sym->is_array = 0; // our grammar actually doesn't arrays in loc_dcl_lists
        //printf("Inserting symbol %s of type %d into dcl_map %p\n", id, sym->type, dcl_map);
        if (!Env_put(dcl_map, id, sym))
            print_error("Ignoring duplicate declaration of "
            "identifier %s.", id);
    }

    Array_free(idx);
}

Env *dcl_map_new() {
    return Env_new(NULL);
}



/* Don't rely on this. Only used for printing errors. */
const char *type_names[] = { "void", "char", "int", "float", "bool", "function", "function prototype" };

const char *_type_str(enum SymType type) {
    return type_names[type];
}

void verify_scope_return() {
    if (current_return_type != TYPE_VOID && !current_scope->has_return_statement)
        print_error("Non-void function has no return statement.");
}

void merge_into_scope(Env * env) {
    void merge(void *k, void **v) {
        _add_to_scope(k, *((Symbol **) v));
    }
    map_apply(&env->table, merge);

    //Env_free(env);
    map_free(&env->table, NULL);
    map_free(&env->prot_table, NULL);
    free(env);
    return;
}
