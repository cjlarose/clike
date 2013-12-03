#include "minunit.h"
#include "instruction.h"

int inst_length(Instruction *inst) {
    int n = 0;
    for (; inst; inst = inst->next, n++);
    return n;
}

char *test_concat_0() {
    Instruction *all = concat_inst(0);
    mu_assert(inst_length(all) == 0, "Wrong size");
    return NULL;
}

char *test_concat_1() {
    Instruction *inst1 = arithmetic_instruction_new();
    Instruction *all = concat_inst(1, inst1);
    mu_assert(inst_length(all) == 1, "Wrong size");
    return NULL;
}

char *test_concat_2() {
    Instruction *inst1 = arithmetic_instruction_new();
    Instruction *inst2 = copy_instruction_new("m1", "m2");
    Instruction *all = concat_inst(2, inst1, inst2);
    mu_assert(inst_length(all) == 2, "Wrong size");
    return NULL;
}

char *test_concat_3() {
    Instruction *inst1 = arithmetic_instruction_new();
    Instruction *inst2 = copy_instruction_new("m1", "m2");
    Instruction *inst3 = load_int_instruction_new(45);
    Instruction *all = concat_inst(3, inst1, inst2, inst3);
    mu_assert(inst_length(all) == 3, "Wrong size");
    return NULL;
}

char *test_null_0() {
    Instruction *inst1 = NULL;
    Instruction *all = concat_inst(1, inst1);
    mu_assert(inst_length(all) == 0, "Wrong size");
    return NULL;
}

char *test_null_1() {
    Instruction *inst1 = arithmetic_instruction_new();
    Instruction *inst2 = copy_instruction_new("m1", "m2");
    Instruction *null_inst = NULL;

    Instruction *all = concat_inst(3, inst1, inst2, null_inst);
    mu_assert(inst_length(all) == 2, "Wrong size");
    return NULL;
}

char *test_null_2() {
    Instruction *inst1 = arithmetic_instruction_new();
    Instruction *inst2 = copy_instruction_new("m1", "m2");
    Instruction *null_inst = NULL;
    Instruction *all = concat_inst(3, inst1, null_inst, inst2);
    mu_assert(inst_length(all) == 2, "Wrong size");
    return NULL;
}

char *test_null_3() {
    Instruction *inst1 = arithmetic_instruction_new();
    Instruction *inst2 = copy_instruction_new("m1", "m2");
    Instruction *null_inst = NULL;
    Instruction *all = concat_inst(3, null_inst, inst1, inst2);
    mu_assert(inst_length(all) == 2, "Wrong size");
    return NULL;
}

char *test_null_4() {
    Instruction *inst1 = arithmetic_instruction_new();
    Instruction *null_inst = NULL;
    Instruction *all = concat_inst(3, null_inst, null_inst, inst1);
    mu_assert(inst_length(all) == 1, "Wrong size");
    return NULL;
}

char *test_null_5() {
    Instruction *inst1 = arithmetic_instruction_new();
    Instruction *null_inst = NULL;
    Instruction *all = concat_inst(3, null_inst, inst1, null_inst);
    mu_assert(inst_length(all) == 1, "Wrong size");
    return NULL;
}

char *test_null_6() {
    Instruction *inst1 = arithmetic_instruction_new();
    Instruction *null_inst = NULL;
    Instruction *all = concat_inst(3, inst1, null_inst, null_inst);
    mu_assert(inst_length(all) == 1, "Wrong size");
    return NULL;
}

char *test_null_7() {
    Instruction *null_inst = NULL;
    Instruction *all = concat_inst(3, null_inst, null_inst, null_inst);
    mu_assert(inst_length(all) == 0, "Wrong size");
    return NULL;
}

char *test_nested_0() {
    Instruction *inst1 = arithmetic_instruction_new();
    Instruction *inst2 = copy_instruction_new("m1", "m2");
    Instruction *inst3 = load_int_instruction_new(45);
    Instruction *all = concat_inst(2, inst1, concat_inst(2, inst2, inst3));
    mu_assert(inst_length(all) == 3, "Wrong size");
    return NULL;
}

char *test_nested_1() {
    Instruction *inst1 = arithmetic_instruction_new();
    Instruction *inst2 = copy_instruction_new("m1", "m2");
    Instruction *inst3 = load_int_instruction_new(45);
    Instruction *all = concat_inst(2, concat_inst(2, inst1, inst2), inst3);
    mu_assert(inst_length(all) == 3, "Wrong size");
    return NULL;
}

char *test_nested_2() {
    Instruction *inst1 = arithmetic_instruction_new();
    Instruction *inst2 = copy_instruction_new("m1", "m2");
    Instruction *inst3 = load_int_instruction_new(45);
    Instruction *inst4 = load_float_instruction_new(45.0);
    Instruction *all = concat_inst(2, 
        concat_inst(2, concat_inst(2, inst1, inst2), inst3), 
        inst4);
    mu_assert(inst_length(all) == 4, "Wrong size");
    return NULL;
}

char *all_tests() {
    mu_suite_start();
    mu_run_test(test_concat_0);
    mu_run_test(test_concat_1);
    mu_run_test(test_concat_2);
    mu_run_test(test_concat_3);
    mu_run_test(test_null_0);
    mu_run_test(test_null_1);
    mu_run_test(test_null_2);
    mu_run_test(test_null_3);
    mu_run_test(test_null_4);
    mu_run_test(test_null_5);
    mu_run_test(test_null_6);
    mu_run_test(test_null_7);
    mu_run_test(test_nested_0);
    mu_run_test(test_nested_1);
    mu_run_test(test_nested_2);
    return NULL;
}

RUN_TESTS(all_tests);
