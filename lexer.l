%{
  #include <iostream>
  #include <cstring>
  #include "parser.tab.h"  
%}

%option noyywrap

%%
BLOCO             { return BLOCK; }
FIM               { return END_BLOCK; }
PRINT             { return PRINT; }

CADEIA            |
NUMERO            { yylval.sval = strdup(yytext); return TYPE; }

_[A-Za-z0-9]+_    { yylval.sval = strdup(yytext); return TK_ID_BLOCK; }
[a-z][a-z0-9_]*   { yylval.sval = strdup(yytext); return TK_ID; }

[0-9]+            |
[0-9]+\.[0-9]+    { yylval.sval = strdup(yytext); return TK_NUMBER; }

\"[^\"]*\"        { yylval.sval = strdup(yytext); return TK_STRING; }
"="               { return EQ; }
","               { return COMMA; }
";"               { return ENDL; }
"+"               { return PLUS; }
[ \t\n]           {}
%%