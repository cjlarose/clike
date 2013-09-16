%option noyywrap
%{
%}

DIGIT  [0-9]
ID      [a-z][a-zA-Z0-9]*
     
%%
{DIGIT}+    { printf("int:%s ", yytext); }
{ID}        { printf("id:%s ", yytext); }
[ \n\t\r\f\v]+  // Ignore whitespace
.           { printf("%s ", yytext); }
%%

int main() {
    yylex();
}
