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

    fprintf(stderr, "Line %d: Error: ", line_num);

    //fprintf(stderr, "%c", yychar);

    switch(yychar) {
        case ')':
            fprintf(stderr, "%s", "Unpected ')'");
            break;
        case ']':
            fprintf(stderr, "%s", "Unexpected ']'");
            break;
        case '}':
            fprintf(stderr, "%s", "Unexpected '}'");
            break;
        case ';':
            fprintf(stderr, "%s", "Premature ';'");
            break;
        default: 
            vfprintf(stderr, s, ap);
    }

    fprintf(stderr, "\n");
}
