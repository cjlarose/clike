#include <stdio.h>
#include "array.h"
#include "env.h"

void print_data(void *k, void **v, void *info) {
    Env *env = info;
    char *id = k;
    Symbol *sym = *((Symbol **) v);
    int i;
    int length = 1;
    if (sym->is_array)
        length = sym->array_length;

    switch (sym->type) {
        case TYPE_CHAR:
        case TYPE_INT:
            printf("%s: ", k);
            for (i = 0; i < length; i++)
                printf(".word ");
            printf("\n");
            break;
        case TYPE_FLOAT:
            printf("%s: ", k);
            for (i = 0; i < length; i++)
                printf(".double ");
            printf("\n");
            break;
        default:
            break;
    }
}

void print_globals(Env *scope) {
    printf(".data\n");
    map_apply(&scope->table, &print_data, scope);
}

void print_mips(Env *global_scope, Array *procedures) {
    printf("MIPS\n");
    print_globals(global_scope);
}
