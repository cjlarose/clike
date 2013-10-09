%{
# include "clike_fn.h"
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

%token EQ_OP 
%token NEQ_OP 
%token GT_OP 
%token GTE_OP 
%token LT_OP 
%token LTE_OP 

%left "||"
%left "&&"
%nonassoc LT_OP LTE_OP GT_OP GTE_OP EQ_OP NEQ_OP
%left '+' '-'
%left '/' '*'
%right '!'

%expect 1 /* That damn dangling else */

%%
prog: | prog dcl_or_func
dcl_or_func: dcl ';' | func

dcl: type dclr_list 
  | VOID f_prot_list

dclr_list: dclr_list ',' dclr |
  | dclr

f_prot_list: f_prot_list ',' f_prot |
  | f_prot

dclr: f_prot | ID | ID '[' INT_CON ']'

f_prot: ID '(' type_list ')'
  | ID '(' ')'
type_list: type | type_list ',' type

func: type ID '(' id_list ')' loc_dcl_list '{' loc_dcl_list opt_stmt_list '}'
  | VOID ID '(' id_list ')' loc_dcl_list '{' loc_dcl_list opt_stmt_list '}'
  | ID '(' id_list ')' loc_dcl_list '{' loc_dcl_list opt_stmt_list '}'
  | type ID '(' ')' loc_dcl_list '{' loc_dcl_list opt_stmt_list '}'
  | VOID ID '(' ')' loc_dcl_list '{' loc_dcl_list opt_stmt_list '}'
  | ID '(' ')' loc_dcl_list '{' loc_dcl_list opt_stmt_list '}'


type: CHAR | INT | FLOAT

loc_dcl_list: | loc_dcl loc_dcl_list
loc_dcl: type id_list ';'

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

expr: un_op expr %prec '-'
  | expr '/' expr
  | expr '*' expr
  | expr '+' expr
  | expr '-' expr
  | comparison_expr
  | expr "&&" expr
  | expr "||" expr
  | invocation
  | id_with_optional_index
  | '(' expr ')'
  | INT_CON
  | FLOAT_CON
opt_expr_list: | expr_list
expr_list: expr | expr_list ',' expr

comparison_expr: expr EQ_OP expr
  | expr NEQ_OP expr
  | expr GT_OP expr
  | expr GTE_OP expr
  | expr LT_OP expr
  | expr LTE_OP expr

invocation: ID '(' opt_expr_list ')' 

id_with_optional_index: ID
  | ID '[' expr ']'

un_op: '-' | '!'

%%

int main(int argc, char **argv) {
# ifdef DEBUG
    yydebug = 1;
# endif
    return yyparse();
}
