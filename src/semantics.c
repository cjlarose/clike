#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include "env.h"
#include "array.h"
#include "procedure.h"
#include "statements.h"

extern int current_type;
extern int prev_type;
extern int current_return_type;
extern Env *current_scope; 
extern int line_num;
extern int status;

void print_error(char *message, ...) {
    va_list ap;
    va_start(ap, message);
    fprintf(stderr, "Line %d: Error: ", line_num);
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
    sym->type_list = tx ? tx : array_new(0, sizeof(enum SymType)); // hack
    _add_to_scope(id, sym);
}

void type_list_insert(Array *tx) {
    //printf("Inserting type %d into type list\n", current_type);
    array_append(tx, &current_type);
}

Array *type_list_new() {
    Array *tx = array_new(0, sizeof(enum SymType));
    type_list_insert(tx);
    return tx;
}

void id_list_insert(Array *idx, char *id) {
    // stored a stack variable in array?
    //char ** ptr = malloc(sizeof(char **));
    //*ptr = id;
    array_append(idx, &id);
    assert(*((char **) array_get(idx, idx->length - 1)) == id);
}

Array *id_list_new(char *id) {
    Array *idx = array_new(0, sizeof(char *));
    id_list_insert(idx, id);
    return idx;
}

void dcl_map_insert(Env *dcl_map, Array *idx) {
    int i;
    for (i = 0; i < idx->length; i++) {
        char *id = *((char **) array_get(idx, i));
        //printf("idx[%d] = %p\n", i, array_get(idx, i));
        Symbol *sym = malloc(sizeof(Symbol));
        sym->type_list = NULL;
        sym->type = current_type;
        sym->is_array = 0; // our grammar actually doesn't arrays in loc_dcl_lists
        //printf("Inserting symbol %s of type %d into dcl_map %p\n", id, sym->type, dcl_map);
        if (!Env_put(dcl_map, id, sym))
            print_error("Ignoring duplicate declaration of "
            "identifier %s.", id);
    }

    array_free(idx);
    free(idx);
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

void _merge_kv_into_scope(void *k, void **v, void *_) {
    _add_to_scope(k, *((Symbol **) v));
}

void merge_into_scope(Env * env) {
    map_apply(&env->table, &_merge_kv_into_scope, NULL);
    //Env_free(env);
    map_free(&env->table, NULL);
    map_free(&env->prot_table, NULL);
    free(env);
    return;
}

/* total hack */
void set_current_type(enum SymType t) {
    prev_type = current_type;
    current_type = t;
}

void procedure_list_free(Array *procs) {
    int i, j;
    for (i = 0; i < procs->length; i++) {
        Procedure *proc = array_get(procs, i);

        // free env
        Env_free(proc->env);
        free(proc->env);

        // free stmts list
        if (proc->stmts) {
            for (j = 0; j < proc->stmts->length; j++)
                stmt_free(*((StmtNodeContainer **) array_get(proc->stmts, j)));
            array_free(proc->stmts);
            free(proc->stmts);
        }
    }
    array_free(procs);
}
