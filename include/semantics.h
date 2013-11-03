#ifndef _SEMANTICS_H_
#define _SEMANTICS_H_
#include "array.h"
void insert_symbol(char *id);
void insert_array_symbol(char *id, int size);
void insert_fn_prot(char *id, Array *tx);

void type_list_insert(Array *tx);
Array *type_list_new();

void id_list_insert(Array *idx, char *id);
Array *id_list_new(char *id);

#endif
