#include <stdio.h>
#include <stdarg.h>
#include "array.h"
#include "env.h"
#include "procedure.h"
#include "instruction.h"
#include "map.h"

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
            printf("%s:", id);
            for (i = 0; i < length; i++)
                printf(" .word");
            printf("\n");
            break;
        case TYPE_FLOAT:
            printf("%s:", id);
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

void print_inst(char *inst, char *format_str, ...) {
    printf("    %-8s", inst);
    va_list ap;
    va_start(ap, format_str);
    vfprintf(stdout, format_str, ap);
    va_end(ap); 
    printf("\n");
}

int get_frame_size(int local_storage, int num_args) {
    int size = 8; // just to store $ra and %fp
    size += local_storage;
    size += 4 * num_args;

    // sweet, now align to double word boundary
    if (size % 8 != 0)
        size += 8 - (size % 8);
    return size;
}

void print_prologue(Procedure *proc, int num_args, int frame_size) {
    print_inst("subu", "$sp, $sp, %d", frame_size);
    print_inst("sw", "$ra, %d($sp)", 4 * num_args + 4);
    print_inst("sw", "$fp, %d($sp)", 4 * num_args);
    print_inst("addiu", "$fp, $sp, %d", frame_size - 4);
}

void print_epilogue(Procedure *proc, int num_args, int frame_size) {
    print_inst("lw", "$ra, %d($sp)", 4 * num_args + 4);
    print_inst("lw", "$fp, %d($sp)", 4 * num_args);
    print_inst("addiu", "$sp, $sp, %d", frame_size);
    print_inst("jr", "$ra");
}


typedef struct LocalAllocInfo {
    int size;
    Map *map;
} LocalAllocInfo;

// map name -> offset from fp
void allocate_local(void *k, void **v, void *data) {
    LocalAllocInfo *info = data;
    Symbol *sym = *((Symbol **) v);
    int size = 0;
    switch (sym->type) {
        case TYPE_CHAR:
        case TYPE_INT:
        case TYPE_BOOL:
            size = 4;
            break;
        case TYPE_FLOAT:
            size = 8;
            break;
        default:
            break;
    }

    if (info->size % size != 0)
        info->size += size - (info->size % size);

    int *size_ptr = malloc(sizeof(int)); // this is embarrasing
    *size_ptr = size;
    map_insert(info->map, k, size_ptr);
    info->size += size;
}

void print_size(void *k, void **v, void *info) {
    char *id = k;
    int *size = *((int **) v);
    printf("%s => %d\n", id, *size);
}

LocalAllocInfo *allocate_locals(Procedure *proc) {
    LocalAllocInfo *info = malloc(sizeof(LocalAllocInfo));
    info->size = 0;
    info->map = map_new(&fnv1_hash, &str_key_eq, 4);
    map_apply(&(proc->env->table), &allocate_local, info);

    //map_apply(info->map, &print_size, NULL);
    return info;
}

void print_procedure(Procedure *proc) {
    int num_args = 4; // TODO: get correct num args
    LocalAllocInfo *locals = allocate_locals(proc);
    int frame_size = get_frame_size(locals->size, num_args);

    printf("%s:\n", proc->id);
    print_prologue(proc, num_args, frame_size);
    printf("CODE\n");
    print_epilogue(proc, num_args, frame_size);
    //Instruction *inst = proc->code;
    //for (; inst; inst = inst->next) {
    //    printf("%d\n", inst->type);
    //}
}

void print_mips(Env *global_scope, Array *procedures) {
    print_globals(global_scope);
    printf("\n");
    printf(".text\n\n");
    int i;
    for (i = 0; i < procedures->length; i++)
        print_procedure((Procedure *) array_get(procedures, i));
}
