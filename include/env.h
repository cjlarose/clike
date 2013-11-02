#include "map.h"
typedef struct Env {
    struct map table;
    struct Env * prev;
} Env;

typedef struct Symbol {
    int type;
} Symbol;

void Env_init(Env * env, Env * prev);
Env * Env_new(Env * prev);
void Env_put(Env * env, char * id, Symbol * sym);
Symbol * Env_get(Env * env, char * id);
