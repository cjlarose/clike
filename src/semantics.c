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
        sym->type = current_type;
        sym->is_array = 0; // our grammar actually doesn't arrays in loc_dcl_lists
        printf("Inserting symbol %s of type %d into dcl_map %p\n", id, sym->type, dcl_map);
        if (!Env_put(dcl_map, id, sym))
            fprintf(stderr, "Line %d: Ignoring duplicate declaration of identifier %s.\n", line_num, id);
    }
}

Env *dcl_map_new() {
    return Env_new(NULL);
}


/* Don't rely on this. Only used for printing errors. */
char *_type_str(enum SymType type) {
    switch(type) {
        case TYPE_VOID:
            return "void";
        case TYPE_CHAR:
            return "char";
        case TYPE_INT:
            return "int";
        case TYPE_FLOAT:
            return "float";
        case TYPE_FN:
            return "function";
        case TYPE_FN_PROT:
            return "function prototype";
    }
}

/* 
 * Given a function idenifier, id_list, and declaration list:
 *   verify every id in declaration list is in the id_list
 *     reject invalid declarations
 *   verify every id in id_list is in declaration list
 *     verify that it is of the correct type according to prototype (if exists)
 *   construct type list
 * 
 *   create symbol entry for fn
 *   add fn to global symbol table
 *   add all (valid) symbols in declaration list to local symbol table
 *   
 */
void verify_fn_dcl(char *fn_id, Array *idx, Env *dclx) {

    // verify every id in decl list is in id list
    void check_id_list(void *k, void **v) {
        // ugly linear search
        int i;
        for (i = 0; i < idx->length; i++) {
            char *id = *((char **) Array_get(idx, i));
            if (strcmp(id, (char *) k) == 0)
                return;
        }
        fprintf(stderr, "Line %d: Variable %s found in declaration of function %s, but not found in identifier list. Continuing without declaration of %s.\n", line_num, (char *) k, fn_id, (char *) k);
        // TODO: actually remove k from table
    }
    map_apply(&dclx->table, check_id_list);

    // if there's a prototype, make sure the id list matches in length
    Symbol *prot = Env_get(current_scope, fn_id);
    if (prot) {
        assert(prot->type == TYPE_FN_PROT);
        if (prot->type_list->length != idx->length) {
            fprintf(stderr, "Line %d: Function %s's prototype specifies %d variables, but %s's paramater list has %d variables. Ignoring prototype of function %s entirely.\n", line_num, fn_id, prot->type_list->length, fn_id, idx->length, fn_id);
            prot = NULL;
        }
    }

    // new type list
    Array *tx = Array_init(idx->length, sizeof(enum SymType));

    // verify every id in id list is in decl list
    //   every such id matches its type in the prototype
    int i;
    for (i = 0; i < idx->length; i++) {
        char *id = *((char **) Array_get(idx, i));
        Symbol *sym = Env_get(dclx, id);
        if (!sym) {
            fprintf(stderr, "Line %d: Variable %s found in parameter list of function %s, but not found in %s's declaration. Assuming %s's type is int.\n", line_num, id, fn_id, fn_id, id);
            sym = malloc(sizeof(Symbol));
            sym->type = TYPE_INT;
            sym->is_array = false;
            Env_put(dclx, id, sym);
        } else if(prot) {
            // matches prototype type?
            int prot_type = *((int *) Array_get(prot->type_list, i));
            if (sym->type != prot_type) {
                fprintf(stderr, "Line: %d: Variable %s declared as %s in %s's declaration, but %s's prototype specifies that %s's type should be %s. Assuming %s's type is %s.\n", line_num, id, _type_str(sym->type), fn_id, fn_id, id, _type_str(prot_type), id, _type_str(sym->type));
            }
        }

        // insert into type list
        Array_set(tx, i, &sym->type);
    }

    /*
    for (i = 0; i < tx->length; i++) {
        int type = *((int *) Array_get(tx, i));
        printf("tx[%d] = %s\n", i, _type_str(type));
    }
    */

}
