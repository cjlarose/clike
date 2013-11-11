#ifndef _ENV_H_
#define _ENV_H_
#include "map.h"
#include "array.h"
typedef struct Env {
    Map table;
    Map prot_table;
    struct Env * prev;
    int has_return_statement;
} Env;

enum SymType {
    TYPE_VOID,
    TYPE_CHAR,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_BOOL,
    TYPE_FN,
    TYPE_FN_PROT
};

typedef struct Symbol {
    enum SymType type;
    int is_array; // 1 if true
    int return_type;
    Array *type_list;
} Symbol;

/*
 * for a function, 
 *   store list of types (to verify invocation)
 *   store return type
 * for a prototype,
 *   store return type
 *   store type list
 */

void Env_init(Env * env, Env * prev);
Env * Env_new(Env * prev);

int Env_put(Env * env, char * id, Symbol * sym);
Symbol * Env_get(Env * env, char * id);
Symbol * Env_get_prot(Env * env, char * id);

int Env_remove(Env *env, char *id);
int Env_remove_prot(Env *env, char *id);

void Env_free(Env * env);

void Symbol_free(Symbol * sym);
#endif
