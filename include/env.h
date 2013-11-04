#ifndef _ENV_H_
#define _ENV_H_
#include "map.h"
#include "array.h"
typedef struct Env {
    struct map table;
    struct map prot_table;
    struct Env * prev;
} Env;

enum SymType {
    TYPE_VOID,
    TYPE_CHAR,
    TYPE_INT,
    TYPE_FLOAT,
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

void Env_free(Env * env);
#endif
