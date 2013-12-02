#include <stdlib.h>
#include <string.h>
#include "env.h"

bool str_key_eq(void *ptr1, void *ptr2) {
    return strcmp((char *) ptr1, (char *) ptr2) == 0;
}

unsigned long long int fnv1_hash (void *ptr) {
    char *str = (char *) ptr;
    unsigned long long int h = 14695981039346656037UL;
    for (; *str != '\0'; str++)
        h = (h * 1099511628211) ^ *str;
    return h;
}

void Env_init(Env * env, Env * prev) {
    env->prev = prev; 
    env->has_return_statement = 0;
    map_init(&env->table, &fnv1_hash, &str_key_eq, 4);
    map_init(&env->prot_table, &fnv1_hash, &str_key_eq, 4);
}

Env * Env_new(Env * prev) {
    Env * env = malloc(sizeof(Env));
    Env_init(env, prev);
    return env;
}

/* 
 * Insert symbol into environment. If the symbol is already in the environment,
 * return 0. Otherwise, insert it and return 1
 */
int Env_put(Env * env, char * id, Symbol * sym) {
    if (map_find(&env->table, id) != NULL)
        return 0;

    if (sym->type != TYPE_FN)
        if (map_find(&env->prot_table, id) != NULL)
            return 0;

    //sym->scope = env;

    if (sym->type == TYPE_FN_PROT)
        map_insert(&env->prot_table, id, sym);
    else
        map_insert(&env->table, id, sym);
    return 1;
}

Symbol * Env_get(Env * env, char * id) {
    void * found;
    for (; env != NULL; env = env->prev) 
        if (( found = map_find(&env->table, id) ) != NULL)
            return *((Symbol **) found);
    return NULL;
}

Symbol * Env_get_prot(Env * env, char * id) {
    void * found;
    for (; env != NULL; env = env->prev) 
        if (( found = map_find(&env->prot_table, id) ) != NULL)
            return *((Symbol **) found);
    return NULL;
}

void Symbol_free(Symbol *sym) {
    if (sym->type_list) {
        array_free(sym->type_list);
        free(sym->type_list);
    }
}

void Entry_free(void * id, void * sym) {
    //free(id);
    Symbol_free(sym);
    free(sym);
}

int Env_remove(Env * env, char * id) {
    return map_delete(&env->table, id, &Entry_free);
}

int Env_remove_prot(Env * env, char * id) {
    return map_delete(&env->prot_table, id, &Entry_free);
}

void Env_free(Env * env) {
    map_free(&env->table, Entry_free);
    map_free(&env->prot_table, Entry_free);
    //free(env);
}
