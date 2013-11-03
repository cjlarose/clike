#ifndef _SEMANTICS_H_
#define _SEMANTICS_H_
#include "array.h"
void insert_symbol(char *id);
void insert_array_symbol(char *id, int size);
Array *new_type_list();
void type_list_insert(Array *tx);
#endif
