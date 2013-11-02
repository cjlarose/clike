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
BINOPS     "->"|"++"|"--"|"<<"|">>"
COMPOPS    "<="|">="|"=="|"!="|">"|"<"
SEPCHARS   ([\(\)\[\]{},;:]|"...")
INTSUFF    ([uU]("l"|"L"|"ll"|"LL")?|("l"|"L"|"ll"|"LL")[uU]?)
EXPONENT   ([Ee][+-]?[0-9]+)
FLOATSUFF  [FfLl]
DOTDIGITS  (([0-9]+"."[0-9]*)|("."[0-9]+))
     
%%
"auto"           // keywords
"_Bool"         
"break"         
"case"          
"char"          {  return CHAR; }
"_Complex"      
"const"         
"continue"      
"default"       
"restrict"      
"do"            
"double"        {  return FLOAT; }
"else"          {  return ELSE; }
"enum"          
"extern"        
"float"         {  return FLOAT; }
"for"           {  return FOR;}
"goto"          {  return GOTO; }
"if"            {  return IF; }
"_Imaginary"    
"inline"        
"int"           {  return INT; }
"long"          
"register"      
"return"        {  return RETURN; }
"short"         
"signed"        
"sizeof"        
"static"        
"struct"        
"switch"        
"typedef"       
"union"         
"unsigned"      
"void"          {  return VOID; }
"volatile"      
"while"         {  return WHILE; }

0[0-7]*{INTSUFF}?           { yylval.ival = atoi(yytext); return INT_CON; } // integer constants
0[Xx][a-fA-F0-9]+{INTSUFF}? { yylval.ival = atoi(yytext); return INT_CON; }
[1-9][0-9]*{INTSUFF}?       { yylval.ival = atoi(yytext); return INT_CON; }

[0-9]+{EXPONENT}{FLOATSUFF}?       { yylval.fval = atof(yytext); return FLOAT_CON; } // floating-point constants
{DOTDIGITS}{EXPONENT}?{FLOATSUFF}? { yylval.fval = atof(yytext); return FLOAT_CON; }

"/*"         BEGIN(comment); // traditional comments
<comment>[^*\n]*
<comment>"*"+[^*/\n]*
<comment>\n             ++line_num;
<comment>"*"+"/"        BEGIN(INITIAL);
<comment><<EOF>>        { fprintf(stderr, "%d, Unterminated Comment\n", line_num); status = 1; BEGIN(INITIAL); }

"//".*  // c++ style comments

"\""(\\.|[^"])*"\"" {  yylval.sval = strdup(yytext); return STRING_LITERAL; } // string constants

"'"(\\.|[^'])*"'" { yylval.sval = strdup(yytext); return CHAR_LITERAL; } // char constants

{ID}        { yylval.sval = strdup(yytext); return ID; } // identifiers

"=" {  return *yytext; }
{OPS}      {  return *yytext; } // operators
{ASSGOPS}    
"||"     {  return LOGICAL_OR; } // operators
"&&"     {  return LOGICAL_AND; } // operators
{BINOPS}    {  return BIN_OP; }
{COMPOPS}    {  return COMP_OP; }
{SEPCHARS}   {  return *yytext; }

"#" 

\n { ++line_num; }
[ \t\r\f\v]+  // Ignore whitespace
.           { fprintf(stderr, "Unknown token:%s\n", yytext); status=1; }
%%

#if TOKENOUT_MAIN
int main(int argc, char **argv) {
    /*
    yylex();
    yylex_destroy();
    return status;
    */
    int tok;

    while ((tok=yylex()) > 0) {
        //printf("tok=%d  yytext=%s\n", tok, yyget_text());
        switch (tok) {
            case CHAR:
                printf("kw:char ");
                break;
            case FLOAT:
                printf("kw:float ");
                break;
            case ELSE:
                printf("kw:else ");
                break;
            case FOR:
                printf("kw:for ");
                break;
            case GOTO:
                printf("kw:goto ");
                break;
            case IF:
                printf("kw:if ");
                break;
            case INT:
                printf("kw:int ");
                break;
            case RETURN:
                printf("kw:return ");
                break;
            case VOID:
                printf("kw:void ");
                break;
            case WHILE:
                printf("kw:while ");
                break;
            case ID:
                printf("id:%s ", yytext);
                break;
            case STRING_LITERAL:
                printf("str:%s ", yytext);
                break;
            case INT_CON:
                printf("int:%s ", yytext);
                break;
            case FLOAT_CON:
                printf("int:%s ", yytext);
                break;
            case CHAR_LITERAL:
                printf("char:%s ", yytext);
                break;
            default:
                printf("%s ", yytext);
                break;
        }
    }

    yylex_destroy();
    return status;
}
#endif
