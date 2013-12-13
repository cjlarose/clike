#include <stdio.h>
#include <stdarg.h>
#include "array.h"
#include "env.h"
#include "procedure.h"
#include "instruction.h"

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

void print_procedure(Procedure *proc) {
    int num_args = 4;
    int frame_size = get_frame_size(0, num_args);

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
