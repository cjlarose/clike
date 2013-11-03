#include "minunit.h"
#include "env.h"

Env *global = NULL;

unsigned long long int hash1 = 0;

char * test_init() {
    global = Env_new(NULL);
    return NULL;
}

char * test_global() {
    Symbol * sym1, * sym2;
    sym1 = (Symbol *) malloc(sizeof(Symbol));
    sym2 = (Symbol *) malloc(sizeof(Symbol));
    sym1->type = 1;
    sym2->type = 2;
    Env_put(global, strdup("foo"), sym1);
    Env_put(global, strdup("bar"), sym2);

    mu_assert(Env_get(global, "foo") == sym1, "Wrong address");
    mu_assert(Env_get(global, "bar") == sym2, "Wrong address");

    mu_assert(Env_get(global, "foo")->type == 1, "Wrong type");
    mu_assert(Env_get(global, "bar")->type == 2, "Wrong type");


    hash1 = fnv1_hash("bar");
    return NULL;
}


char * test_local() {
    Env * local_scope = Env_new(global);
    Symbol local_sym1, local_sym2;
    local_sym1.type = 3;
    local_sym2.type = 4;

    Env_put(local_scope, strdup("baz"), &local_sym1); // new id name
    Env_put(local_scope, strdup("foo"), &local_sym2); // already in global

    mu_assert(Env_get(local_scope, "baz")->type == 3, "Wrong type");
    mu_assert(Env_get(local_scope, "foo")->type == 4, "Wrong type");
    
    mu_assert(hash1 == fnv1_hash("bar"), "WTF");
    // test inheritance
    mu_assert(Env_get(local_scope, "bar") != NULL, "Cannot find global symbol");
    //mu_assert(Env_get(local_scope, "bar")->type == 2, "Wrong type");
    return NULL;
}

char * all_tests() {
    mu_suite_start();
    mu_run_test(test_init);
    mu_run_test(test_global);
    mu_run_test(test_local);

    Env_free(global);
    return NULL;
}

RUN_TESTS(all_tests);
