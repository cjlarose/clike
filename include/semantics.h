#ifndef _SEMANTICS_H_
#define _SEMANTICS_H_
#include "array.h"
#include "env.h"
void print_error(char *message, ...);
void insert_symbol(char *id);
void insert_array_symbol(char *id, int size);
void insert_fn_prot(char *id, Array *tx);

void type_list_insert(Array *tx);
Array *type_list_new();

void id_list_insert(Array *idx, char *id);
Array *id_list_new(char *id);

void dcl_map_insert(Env *dcl_map, Array *idx);
Env *dcl_map_new();

void _add_to_scope(char *id, Symbol *sym);
const char *_type_str(enum SymType type);
void verify_scope_return();

void merge_into_scope(Env *env);

#endif
