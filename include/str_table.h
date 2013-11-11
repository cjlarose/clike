#include "map.h"

typedef struct StringTable {
    Map table;
} StringTable;

void str_table_init(StringTable *);
StringTable *str_table_new();

// if in table, return
// else, put copy in table, return
char *str_table_get(StringTable *, char *str);
