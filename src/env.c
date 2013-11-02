#include <stdlib.h>
#include "env.h"

bool key_eq(void *ptr1, void *ptr2) {
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
    map_init(&(env->table), &fnv1_hash, &key_eq, 4);
}

Env * Env_new(Env * prev) {
    Env * env = malloc(sizeof(Env));
    Env_init(env, prev);
}

void Env_put(Env * env, char * id, Symbol * sym) {
    map_insert(&(env->table), id, sym);
}

Symbol * Env_get(Env * env, char * id) {
    Env * e;
    Symbol * found;
    for (e = env; e != NULL; e = e->prev) 
        if (( found = *((Symbol **) map_find(&(env->table), id)) ) != NULL)
            return found;
    return NULL;
}

void Env_free(Env * env) {
    map_free(&(env->table), NULL);
    free(env);
}
