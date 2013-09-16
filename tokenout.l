%option main
%x comment

%{
int line_num = 1;
%}

ID     [a-zA-Z_][a-zA-Z0-9_]*
OPS    "!"|"%"|"^"|"&"|"*"|"-"|"+"|"="|"~"|"|" 
ASSGOPS ("+"|"-"|"*"|"/"|"%"|"<<"|">>"|"&"|"^"|"|")"="
COMPOPS "->"|"++"|"--"|"<<"|">>"|"<="|">="|"=="|"!="|"&&"|"||"
INTSUF ([uU]("l"|"L"|"ll"|"LL")?|("l"|"L"|"ll"|"LL")[uU]?)
     
%%
"auto"          { printf("kw:%s ", yytext); }
"_Bool"         { printf("kw:%s ", yytext); }
"break"         { printf("kw:%s ", yytext); }
"case"          { printf("kw:%s ", yytext); }
"char"          { printf("kw:%s ", yytext); }
"_Complex"          { printf("kw:%s ", yytext); }
"const"         { printf("kw:%s ", yytext); }
"continue"          { printf("kw:%s ", yytext); }
"default"           { printf("kw:%s ", yytext); }
"restrict"          { printf("kw:%s ", yytext); }
"do"            { printf("kw:%s ", yytext); }
"double"            { printf("kw:%s ", yytext); }
"else"          { printf("kw:%s ", yytext); }
"enum"          { printf("kw:%s ", yytext); }
"extern"            { printf("kw:%s ", yytext); }
"float"         { printf("kw:%s ", yytext); }
"for"           { printf("kw:%s ", yytext); }
"goto"          { printf("kw:%s ", yytext); }
"if"            { printf("kw:%s ", yytext); }
"_Imaginary"            { printf("kw:%s ", yytext); }
"inline"            { printf("kw:%s ", yytext); }
"int"           { printf("kw:%s ", yytext); }
"long"          { printf("kw:%s ", yytext); }
"register"          { printf("kw:%s ", yytext); }
"return"            { printf("kw:%s ", yytext); }
"short"         { printf("kw:%s ", yytext); }
"signed"            { printf("kw:%s ", yytext); }
"sizeof"            { printf("kw:%s ", yytext); }
"static"            { printf("kw:%s ", yytext); }
"struct"            { printf("kw:%s ", yytext); }
"switch"            { printf("kw:%s ", yytext); }
"typedef"           { printf("kw:%s ", yytext); }
"union"         { printf("kw:%s ", yytext); }
"unsigned"          { printf("kw:%s ", yytext); }
"void"          { printf("kw:%s ", yytext); }
"volatile"          { printf("kw:%s ", yytext); }
"while"         { printf("kw:%s ", yytext); }

0[0-7]*{INTSUF}? { printf("oint:%s ", yytext); } // integer constants
0[Xx][a-fA-F0-9]+{INTSUF}? { printf("hint:%s ", yytext); }
[1-9][0-9]*{INTSUF}? { printf("decint:%s ", yytext); }

"/*"         BEGIN(comment); // traditional C comments
<comment>[^*\n]*
<comment>"*"+[^*/\n]*
<comment>\n             ++line_num;
<comment>"*"+"/"        BEGIN(INITIAL);
<comment><<EOF>>        { fprintf(stderr, "%d, Unterminated Comment\n", line_num); BEGIN(INITIAL); }

{ID}        { printf("id:%s ", yytext); }

{OPS}        { printf("%s ", yytext); }
{ASSGOPS}        { printf("%s ", yytext); }
{COMPOPS}        { printf("%s ", yytext); }

\n { ++line_num; }
[ \t\r\f\v]+  // Ignore whitespace
.           { printf("%s ", yytext); }
%%
