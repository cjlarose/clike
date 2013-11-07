%{
#include <stdlib.h>
#include <stdio.h>
#include "clike_fn.h"
#include "semantics.h"
#include "declarations.h"
#include "expressions.h"
#include "env.h"

int status;
Env *current_scope;
enum SymType prev_type; 
enum SymType current_type; 
enum SymType current_return_type;

%}

%union {
    int ival;
    float fval;
    char *sval;
    void *ptrval; // TODO: remove this.
    struct ExpNode * exp_node;
    struct Array * arr_val;
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

%token <sval> COMP_OP
%token BIN_OP
%token <sval> LOGICAL_OR
%token <sval> LOGICAL_AND

%token ASSIGNMENT_OP /* ignored compound assignment operators */

%left LOGICAL_OR
%left LOGICAL_AND
%nonassoc COMP_OP
%left '+' '-'
%left '/' '*'
%right '!'

%token <sval> '!'
%token <sval> '-'
%token <sval> '+'
%token <sval> '*'
%token <sval> '/'

%type <ival> int_con
%type <ptrval> type_list
%type <ptrval> id_list
%type <ptrval> loc_dcl_list
%type <ptrval> loc_dcl
%type <ptrval> func_begin
%type <exp_node> expr
%type <arr_val> expr_list
%type <arr_val> opt_expr_list
%type <exp_node> opt_expr
%type <exp_node> id_with_optional_index

%expect 1 /* That damn dangling else */

%%
prog: | prog {current_return_type = TYPE_INT; } dcl_or_func
dcl_or_func: dcl ';' | func

void: VOID {set_current_type(TYPE_VOID); }

dcl: type dclr_list  
  | void f_prot_list 

dclr_list: dclr_list ',' dclr |
  | dclr

f_prot_list: f_prot_list ',' f_prot |
  | f_prot

dclr: f_prot 
  | ID { insert_symbol($1); } 
  | ID '[' int_con ']' { insert_array_symbol($1, $3); }

f_prot: ID '(' type_list ')' { insert_fn_prot($1, $3); current_type = 0; }
  | ID '(' ')' { current_return_type = current_type; insert_fn_prot($1, NULL); }

type_list: type { current_return_type = prev_type; $$ = type_list_new(); } 
  | type_list ',' type {type_list_insert($1); $$ = $1; }

func_begin: type ID '(' id_list ')' {current_return_type = current_type;} loc_dcl_list { $$ = validate_fn_dcl($2, $4, $7); } 
  | void ID '(' id_list ')' {current_return_type = current_type;} loc_dcl_list { $$ = validate_fn_dcl($2, $4, $7); }
  | ID '(' id_list ')' loc_dcl_list { $$ = validate_fn_dcl($1, $3, $5); }
  | type ID '(' ')' {current_return_type = current_type;} loc_dcl_list { $$ = validate_fn_dcl($2, NULL, $6); }
  | void ID '(' ')' { current_return_type = current_type; } loc_dcl_list { $$ = validate_fn_dcl($2, NULL, $6); }
  | ID '(' ')' loc_dcl_list { $$ = validate_fn_dcl($1, NULL, $4); } 

func: func_begin { current_scope = $1; } '{' loc_dcl_list { Env_merge_into(current_scope, $4); } opt_stmt_list '}'{ /* TODO: Verify fn body contains at least one return expr if non void (store on scope?) */ current_scope = current_scope->prev; }

type: CHAR {set_current_type(TYPE_CHAR); } | INT {set_current_type(TYPE_INT);} | FLOAT {set_current_type(TYPE_FLOAT);}

loc_dcl_list: { $$ = dcl_map_new(); } 
  | loc_dcl_list loc_dcl { dcl_map_insert($1, $2); $$ = $1; }
loc_dcl: type id_list ';' { $$ = $2; }

id_list: ID { $$ = id_list_new($1); }
  | id_list ',' ID { id_list_insert($1, $3); }

stmt: IF '(' expr ')' stmt { validate_boolean_expression($3); }
  | IF '(' expr ')' stmt ELSE stmt { validate_boolean_expression($3); }
  | WHILE '(' expr ')' opt_stmt { validate_boolean_expression($3); }
  | FOR '(' opt_assg ';' opt_expr ';' opt_assg ')' opt_stmt { validate_boolean_expression($5); }
  | RETURN opt_expr { validate_return_statement($2); }
  | assg
  | ID '(' opt_expr_list ')' { /* TODO: make sure void  */ }
  | '{' opt_stmt_list '}'
 
opt_assg: | assg
opt_expr: {$$ = NULL; } | expr {$$ = $1; }
opt_stmt: | stmt

opt_stmt_list: | stmt_list
stmt_list: stmt ';' | stmt_list stmt ';'

assg: id_with_optional_index '=' expr { validate_assignment($1, $3); } 

expr: '-' expr %prec '-' { $$ = new_arithmetic_expnode($1, $2, NULL); }
  | '!' expr %prec '-' { $$ = new_boolean_expnode($1, $2, NULL); }
  | expr '/' expr { $$ = new_arithmetic_expnode($2, $1, $3); }
  | expr '*' expr { $$ = new_arithmetic_expnode($2, $1, $3); }
  | expr '+' expr { $$ = new_arithmetic_expnode($2, $1, $3); }
  | expr '-' expr { $$ = new_arithmetic_expnode($2, $1, $3); }
  | expr COMP_OP expr { $$ = new_comparison_expnode($2, $1, $3); }
  | expr LOGICAL_AND expr { $$ = new_boolean_expnode($2, $1, $3); }
  | expr LOGICAL_OR expr { $$ = new_boolean_expnode($2, $1, $3); }
  | ID '(' opt_expr_list ')' { $$ = new_invocation_expnode($1, $3, 0); }
  | id_with_optional_index { $$ = $1; }
  | '(' expr ')' { $$ = $2; }
  | int_con { $$ = new_int_expnode(); }
  | FLOAT_CON { $$ = new_float_expnode(); }

id_with_optional_index: ID { $$ = new_id_expnode($1, NULL); }
  | ID '[' expr ']' { $$ = new_id_expnode($1, $3); }

opt_expr_list: { $$ = NULL; } | expr_list { $$ = $1; }
expr_list: expr { $$ = expr_list_new($1); } 
  | expr_list ',' expr { expr_list_insert($1, $3); }

int_con: OCT_INT_CON | HEX_INT_CON | DEC_INT_CON 

%%

/* total hack */
set_current_type(enum SymType t) {
    prev_type = current_type;
    current_type = t;
}

#ifndef TOKENOUT_MAIN
int main(int argc, char **argv) {

    current_scope = Env_new(NULL);

# ifdef DEBUG
    yydebug = 1;
# endif
    int bison_status = yyparse();
    if (bison_status)
        status = bison_status;
    yylex_destroy();
    return status;
}
#endif
