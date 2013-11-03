%{
#include <stdlib.h>
#include <stdio.h>
#include "clike_fn.h"
#include "semantics.h"
#include "env.h"

Env *current_scope;
enum SymType current_type; 

%}

%union {
    int ival;
    float fval;
    char *sval;
    void *ptrval;
}

%token <ival> DEC_INT_CON /* constants */
%token <ival> OCT_INT_CON 
%token <ival> HEX_INT_CON 
%token <fval> FLOAT_CON
%token <sval> CHAR_LITERAL
%token <sval> STRING_LITERAL

%token CHAR /* types */
%token INT
%token FLOAT
%token VOID

%token KEYWORD /* Ignored keywords */

%token <sval> ID

%token IF /* control flow */
%token ELSE
%token WHILE
%token FOR
%token RETURN
%token GOTO

%token COMP_OP
%token BIN_OP
%token LOGICAL_OR
%token LOGICAL_AND

%token ASSIGNMENT_OP /* ignored compound assignment operators */

%left LOGICAL_OR
%left LOGICAL_AND
%nonassoc COMP_OP
%left '+' '-'
%left '/' '*'
%right '!'

%type <ival> int_con
%type <ptrval> type_list

%expect 1 /* That damn dangling else */

%%
prog: | prog dcl_or_func
dcl_or_func: dcl ';' | func

void: VOID {current_type = TYPE_VOID; }

dcl: type dclr_list  
  | void f_prot_list 

dclr_list: dclr_list ',' dclr |
  | dclr

f_prot_list: f_prot_list ',' f_prot |
  | f_prot

dclr: f_prot 
  | ID { insert_symbol($1); } 
  | ID '[' int_con ']' { insert_array_symbol($1, $3); }

f_prot: ID '(' type_list ')' { insert_fn_prot($1, $3); }
  | ID '(' ')' { insert_fn_prot($1, NULL); }
type_list: type { $$ = new_type_list(); } 
  | type_list ',' type {type_list_insert($1); $$ = $1; }

func: type ID '(' id_list ')' loc_dcl_list '{' loc_dcl_list opt_stmt_list '}'
  | void ID '(' id_list ')' loc_dcl_list '{' loc_dcl_list opt_stmt_list '}'
  | ID '(' id_list ')' loc_dcl_list '{' loc_dcl_list opt_stmt_list '}'
  | type ID '(' ')' loc_dcl_list '{' loc_dcl_list opt_stmt_list '}'
  | void ID '(' ')' loc_dcl_list '{' loc_dcl_list opt_stmt_list '}'
  | ID '(' ')' loc_dcl_list '{' loc_dcl_list opt_stmt_list '}'


type: CHAR {current_type = TYPE_CHAR; } | INT {current_type = TYPE_INT;} | FLOAT {current_type = TYPE_FLOAT;}

loc_dcl_list: | loc_dcl_list loc_dcl
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
  | expr COMP_OP expr
  | expr LOGICAL_AND expr
  | expr LOGICAL_OR expr
  | invocation
  | id_with_optional_index
  | '(' expr ')'
  | int_con
  | FLOAT_CON
opt_expr_list: | expr_list
expr_list: expr | expr_list ',' expr


invocation: ID '(' opt_expr_list ')' 

id_with_optional_index: ID
  | ID '[' expr ']'

un_op: '-' | '!'

int_con: OCT_INT_CON | HEX_INT_CON | DEC_INT_CON 

%%

#ifndef TOKENOUT_MAIN
int main(int argc, char **argv) {

    current_scope = Env_new(NULL);

# ifdef DEBUG
    yydebug = 1;
# endif
    int status;
    status = yyparse();
    yylex_destroy();
    return status;
}
#endif
