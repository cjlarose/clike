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

loc_dcl: type id_list

id_list: ID | id_list ',' ID

stmt: IF '(' expr ')' stmt
  | IF '(' expr ')' stmt ELSE stmt
  | WHILE '(' expr ')' opt_stmt
  | FOR '(' opt_assg ';' opt_expr ';' opt_assg ')' opt_stmt
  | RETURN opt_expr
  | assg
  | invocation
  | '{' opt_stmt_list '}'
 
opt_assg: | assg
opt_expr: | expr
opt_stmt: | stmt

opt_stmt_list: | stmt_list
stmt_list: stmt ';' | stmt_list stmt ';'

assg: id_with_optional_index '=' expr

id_with_optional_index: ID
  | ID '[' expr ']'

invocation: ID '(' opt_expr_list ')' 

expr: un_op expr
  | expr bin_op expr
  | invocation
  | id_with_optional_index
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
