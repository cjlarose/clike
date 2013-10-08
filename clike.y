%{
# include <stdio.h>
%}

%token INTCON /* constants */
%token FLOATCON

%token CHAR /* types */
%token INT
%token FLOAT
%token VOID

%token ID

%token IF /* control flow */
%token ELSE
%token WHILE
%token FOR
%token RETURN

%%
term: INTCON 
%%

int hello() {return 0;}
void yyerror (char *s, ... ) {
    //fprintf(stderr, "%d: error: ", yylineno);
    return;
}
