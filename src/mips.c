#include <stdio.h>
#include "array.h"
#include "env.h"
#include "procedure.h"

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
            printf("%s:", k);
            for (i = 0; i < length; i++)
                printf(" .word");
            printf("\n");
            break;
        case TYPE_FLOAT:
            printf("%s:", k);
            for (i = 0; i < length; i++)
                printf(" .double");
            printf("\n");
            break;
        default:
            break;
    }
}

void print_globals(Env *scope) {
    printf(".data\n\n");
    map_apply(&scope->table, &print_data, scope);
}

void print_procedure(Procedure *proc) {
    printf("%s:\n", proc->id);
}

void print_mips(Env *global_scope, Array *procedures) {
    print_globals(global_scope);
    printf("\n");
    printf(".text\n\n");
    int i;
    for (i = 0; i < procedures->length; i++)
        print_procedure((Procedure *) array_get(procedures, i));
}
