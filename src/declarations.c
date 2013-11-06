#include <stdio.h>
#include <assert.h>
#include "semantics.h"
#include "env.h"
#include "array.h"
extern int current_return_type;
extern Env *current_scope; 

/*****************************************************************************
 * Function Declaration Validation                                           *
 *****************************************************************************/

void validate_dcl_list(char *fn_id, Array *idx, Env *dclx) {
    Array *to_remove = Array_init(0, sizeof(char **));
    // verify every id in decl list is in id list
    void check_id_list(void *k, void **v) {
        // ugly linear search
        int i;
        for (i = 0; i < idx->length; i++) {
            char *id = *((char **) Array_get(idx, i));
            if (strcmp(id, (char *) k) == 0)
                return;
        }
        print_error("Variable %s found in declaration of function "
        "%s, but not found in identifier list. Continuing without declaration "
        "of %s.\n", (char *) k, fn_id, (char *) k);
        Array_append(to_remove, &k);
    }
    map_apply(&dclx->table, &check_id_list);

    int i;
    for (i = 0; i < to_remove->length; i++)
        Env_remove(dclx, *((char **) Array_get(to_remove, i)));
}

Symbol *validate_fn_against_prot(char *fn_id, Array *idx, Symbol *prot) {
    assert(prot->type == TYPE_FN_PROT);
    if (prot->type_list->length != idx->length) {
        print_error("Function %s's prototype specifies %d "
        "variables, but %s's paramater list has %d variables. Ignoring "
        "prototype of function %s entirely.\n", fn_id, 
        prot->type_list->length, fn_id, idx->length, fn_id);
        return NULL;
    }
    printf("PROT RETURN TYPE: %s\n", _type_str(prot->return_type));
    if (prot->return_type != current_return_type) {
        print_error("Function %s's prototype specifies return "
        "type %s, but %s's definition specifies return type %s. Ignoring "
        "prototype of function %s entirely.\n", fn_id, 
        _type_str(prot->return_type), fn_id, _type_str(current_return_type), 
        fn_id);
        return NULL;
    }
    return prot;
}

Array *validate_id_list(char *fn_id, Array *idx, Env *dclx, Symbol *prot) {
    // new type list
    Array *tx = Array_init(idx->length, sizeof(enum SymType));

    // verify every id in id list is in decl list
    //   every such id matches its type in the prototype
    int i;
    for (i = 0; i < idx->length; i++) {
        char *id = *((char **) Array_get(idx, i));
        Symbol *sym = Env_get(dclx, id);
        if (!sym) {
            print_error("Variable %s found in parameter list of "
            "function %s, but not found in %s's declaration. Assuming %s's "
            "type is int.\n", id, fn_id, fn_id, id);
            sym = malloc(sizeof(Symbol));
            sym->type = TYPE_INT;
            sym->is_array = false;
            Env_put(dclx, id, sym);
        } else if(prot) {
            // matches prototype type?
            int prot_type = *((int *) Array_get(prot->type_list, i));
            if (sym->type != prot_type) {
                print_error("Variable %s declared as %s in %s's "
                "declaration, but %s's prototype specifies that %s's type "
                "should be %s. Assuming %s's type is %s.\n", id, 
                _type_str(sym->type), fn_id, fn_id, id, _type_str(prot_type), 
                id, _type_str(sym->type));
            }
        }

        // insert into type list
        Array_set(tx, i, &sym->type);
    }
    return tx;
}

void insert_fn_into_global_symtable(char *fn_id, Array *tx) {
    Symbol * sym = malloc(sizeof(Symbol));
    sym->type = TYPE_FN;
    sym->is_array = 0;
    sym->return_type = current_return_type;
    sym->type_list = tx;
    _add_to_scope(fn_id, sym);
}

/* 
 * Given a function idenifier, id_list, and declaration list:
 *   verify every id in declaration list is in the id_list
 *     reject invalid declarations
 *   verify every id in id_list is in declaration list
 *     verify that it is of the correct type according to prototype (if exists)
 *   construct type list
 * 
 *   create symbol table entry for fn
 *   //add all (valid) symbols in declaration list to local symbol table
 *   
 */
Env *validate_fn_dcl(char *fn_id, Array *idx, Env *dclx) {

    // hack
    if (!idx)
        idx = Array_init(0, sizeof(char *));

    validate_dcl_list(fn_id, idx, dclx);

    // if there's a prototype, make sure the id list matches in length
    Symbol *prot = Env_get_prot(current_scope, fn_id);
    if (prot) {
        if ((prot = validate_fn_against_prot(fn_id, idx, prot)) == NULL)
            Env_remove_prot(current_scope, fn_id);
    }

    Array *tx = validate_id_list(fn_id, idx, dclx, prot);

    insert_fn_into_global_symtable(fn_id, tx);

    /*
    printf("DCLX\n");
    void print_map(void *k, void **v) {
        Symbol *sym = *((Symbol **) v);
        printf("%s => %s\n", (char*) k, _type_str(sym->type));
    }
    map_apply(&dclx->table, &print_map);
    */
    dclx->prev = current_scope;

    return dclx;

    /*
    int i;
    for (i = 0; i < tx->length; i++) {
        int type = *((int *) Array_get(tx, i));
        printf("tx[%d] = %s\n", i, _type_str(type));
    }
    */
}
