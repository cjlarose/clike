%option noyywrap
%x comment
%x str

%{
# include "clike.tab.h"
# include "clike_fn.h"
int line_num = 1;
int status = 0;
%}

ID         [a-zA-Z_][a-zA-Z0-9_]*
OPS        [!%^&*\-+=~\|\./?]
ASSGOPS    ("+"|"-"|"*"|"/"|"%"|"<<"|">>"|"&"|"^"|"|")"="
COMPOPS    "->"|"++"|"--"|"<<"|">>"|"<="|">="|"=="|"!="|"&&"|"||"
SEPCHARS   ([\(\)\[\]{},;:]|"...")
INTSUFF    ([uU]("l"|"L"|"ll"|"LL")?|("l"|"L"|"ll"|"LL")[uU]?)
EXPONENT   ([Ee][+-]?[0-9]+)
FLOATSUFF  [FfLl]
DOTDIGITS  (([0-9]+"."[0-9]*)|("."[0-9]+))
     
%%
"auto"          { printf("kw:%s ", yytext); } // keywords
"_Bool"         { printf("kw:%s ", yytext); }
"break"         { printf("kw:%s ", yytext); }
"case"          { printf("kw:%s ", yytext); }
"char"          { printf("kw:%s ", yytext); return CHAR; }
"_Complex"      { printf("kw:%s ", yytext); }
"const"         { printf("kw:%s ", yytext); }
"continue"      { printf("kw:%s ", yytext); }
"default"       { printf("kw:%s ", yytext); }
"restrict"      { printf("kw:%s ", yytext); }
"do"            { printf("kw:%s ", yytext); }
"double"        { printf("kw:%s ", yytext); return FLOAT; }
"else"          { printf("kw:%s ", yytext); return ELSE; }
"enum"          { printf("kw:%s ", yytext); }
"extern"        { printf("kw:%s ", yytext); }
"float"         { printf("kw:%s ", yytext); return FLOAT; }
"for"           { printf("kw:%s ", yytext); }
"goto"          { printf("kw:%s ", yytext); }
"if"            { printf("kw:%s ", yytext); return IF; }
"_Imaginary"    { printf("kw:%s ", yytext); }
"inline"        { printf("kw:%s ", yytext); }
"int"           { printf("kw:%s ", yytext); return INT; }
"long"          { printf("kw:%s ", yytext); }
"register"      { printf("kw:%s ", yytext); }
"return"        { printf("kw:%s ", yytext); return RETURN; }
"short"         { printf("kw:%s ", yytext); }
"signed"        { printf("kw:%s ", yytext); }
"sizeof"        { printf("kw:%s ", yytext); }
"static"        { printf("kw:%s ", yytext); }
"struct"        { printf("kw:%s ", yytext); }
"switch"        { printf("kw:%s ", yytext); }
"typedef"       { printf("kw:%s ", yytext); }
"union"         { printf("kw:%s ", yytext); }
"unsigned"      { printf("kw:%s ", yytext); }
"void"          { printf("kw:%s ", yytext); return VOID; }
"volatile"      { printf("kw:%s ", yytext); }
"while"         { printf("kw:%s ", yytext); return WHILE; }

0[0-7]*{INTSUFF}?           { printf("octint:%s ", yytext); return INT_CON; } // integer constants
0[Xx][a-fA-F0-9]+{INTSUFF}? { printf("hexint:%s ", yytext); return INT_CON; }
[1-9][0-9]*{INTSUFF}?       { printf("int:%s ", yytext); return INT_CON; }

[0-9]+{EXPONENT}{FLOATSUFF}?       { printf("float:%s ", yytext); return FLOAT_CON; } // floating-point constants
{DOTDIGITS}{EXPONENT}?{FLOATSUFF}? { printf("float:%s ", yytext); return FLOAT_CON; }

"/*"         BEGIN(comment); // traditional comments
<comment>[^*\n]*
<comment>"*"+[^*/\n]*
<comment>\n             ++line_num;
<comment>"*"+"/"        BEGIN(INITIAL);
<comment><<EOF>>        { fprintf(stderr, "%d, Unterminated Comment\n", line_num); status = 1; BEGIN(INITIAL); }

"//".*  // c++ style comments

"\""(\\.|[^"])*"\"" { printf("str:%s ", yytext); } // string constants

"'"(\\.|[^'])*"'" { printf("char:%s ", yytext); } // char constants

{ID}        { printf("id:%s ", yytext); return ID; } // identifiers

"=" { printf("%s ", yytext); return *yytext; }
{OPS}      { printf("%s ", yytext); return *yytext; } // operators
{ASSGOPS}    { printf("%s ", yytext); }
"=="    { printf("%s ", yytext); return EQ_OP; }
"!="    { printf("%s ", yytext); return NEQ_OP; }
">"    { printf("%s ", yytext); return GT_OP; }
">="    { printf("%s ", yytext); return GTE_OP; }
"<"    { printf("%s ", yytext); return LT_OP; }
"<="    { printf("%s ", yytext); return LTE_OP; }
{SEPCHARS}   { printf("%s ", yytext); return *yytext; }

"#" { printf("%s ", yytext); } // not actually a token, but reference implementation accepts it.

\n { ++line_num; }
[ \t\r\f\v]+  // Ignore whitespace
.           { fprintf(stderr, "Unknown token:%s\n", yytext); status=1; }
%%

/*
int main(int argc, char **argv) {
    yylex();
    yylex_destroy();
    return status;
}
*/
