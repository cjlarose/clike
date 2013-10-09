#include <stdio.h>
#include <stdarg.h>
#include "clike_fn.h"

/*
void yyerror (char *s, ... ) {
    fprintf(stderr, "%d: error: ", yylineno);
    return;
}
*/

void yyerror(char *s, ...) {
    va_list ap;
    va_start(ap, s);

    fprintf(stderr, "%d: error: ", line_num);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");
}
