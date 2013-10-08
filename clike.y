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

%token UN_OP
%token BIN_OP

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

expr: un_op expr
  | expr bin_op expr
  | ID
  | ID '(' opt_expr_list ')' 
  | ID '[' expr ']'
  | '(' expr ')'
  | INT_CON
  | FLOAT_CON
opt_expr_list: | expr_list
expr_list: expr | expr_list ',' expr

un_op: UN_OP
bin_op: BIN_OP
%%

void yyerror (char *s, ... ) {
    //fprintf(stderr, "%d: error: ", yylineno);
    return;
}
