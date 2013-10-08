#include <stdio.h>
#include "clike_fn.h"

void yyerror (char *s, ... ) {
    fprintf(stderr, "%d: error: ", yylineno);
    return;
}
