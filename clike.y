%{
# include <stdio.h>
%}

%token INT_CON /* constants */
%token FLOAT_CON

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
dcl: type dclr_list 
  | VOID f_prot_list

dclr_list: dclr 
  | ',' dclr_list

f_prot_list: f_prot 
  | ',' f_prot_list

dclr: f_prot | ID | ID '[' INT_CON ']'

f_prot: ID '(' opt_type_list ')'
opt_type_list: | type_list
type_list: type | type_list ',' type

type: CHAR | INT | FLOAT

%%

void yyerror (char *s, ... ) {
    //fprintf(stderr, "%d: error: ", yylineno);
    return;
}
