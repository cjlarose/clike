#include <stdlib.h>
#include <string.h>
#include "str_table.h"
#include "map.h"
#include "env.h"

void str_table_init(StringTable *str_table) {
    map_init(&str_table->table, &fnv1_hash, &str_key_eq, 4);
}

StringTable *str_table_new() {
    StringTable *new_table = malloc(sizeof(StringTable));
    str_table_init(new_table);
    return new_table;
}

char *str_table_get(StringTable *str_table, char * str) {
    char **in_map = (char **) map_find(&str_table->table, str);
    if (in_map)
        return *in_map;
    else {
        char *new_str = strdup(str);
        map_insert(&str_table->table, new_str, new_str);
        return new_str;
    }
}

void free_entry(void *k, void *v) {
    free(k);
}

void str_table_free(StringTable *str_table) {
    map_free(str_table, &free_entry);
}
