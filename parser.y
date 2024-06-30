%{
  #include <iostream>
  #include <cstring>
  #include "interp.h"

  extern int yylex();
  extern int yyparse();
  extern FILE *yyin;
 
  void yyerror(const char *s);
  std::list<Var> vars;  
%}

%union {
  char *sval;
}

%token <sval> TK_NUMBER TK_STRING TK_ID TK_ID_BLOCK TYPE 
%token BLOCK END_BLOCK PRINT PLUS EQ NUMBER ENDL COMMA 

%type <sval> expr operand

%%
program: list_stmts ;

list_stmts: 
  stmt 
  | list_stmts stmt;   

stmt:
  decl ENDL 
  | assig ENDL                
  | BLOCK TK_ID_BLOCK           { enterScope($2); free($2); }
  | END_BLOCK TK_ID_BLOCK       { exitScope($2);  free($2); }
  | PRINT TK_ID ENDL            { print($2);      free($2); }
; 

decl:
  TYPE list_decls               { defineMany($1, vars); free($1)}
;

list_decls:
  decl_assig                    
  | list_decls COMMA decl_assig 
;

assig:
  TK_ID EQ expr               {
    if($3[0] != '@') {
      Var l = lookup($1);
      if(!l.id.empty()) {
        if(l.type == expressionType($3)) assign($1, $3);
        else std::cerr << "Erro: Tipo de dado incompativel entre identificador e expressao." << std::endl;
      }else
        std::cerr << "Erro: Identificador '" << $1 << "' nao foi declarado." << std::endl;
    } else 
      std::cerr << "Erro: Tipo de dado incompativel entre operadores da expressao." << std::endl;
    
    free($1); 
    free($3);
  }
;

decl_assig:
  TK_ID                         { vars.push_back(Var("", $1, "")); }
  | TK_ID EQ expr               { 
    if($3[0] != '@')   
      vars.push_back(Var(expressionType($3), $1, $3));  
    else 
      std::cerr << "Erro: Tipo de dado incompativel entre operadores da expressao." << std::endl;
    free($1); 
    free($3);
  }
;

expr:
  operand                         { $$ = strdup($1); free($1); }
  | operand PLUS operand          {
    if($1[0] != '@' && $3[0] != '@') {
      bool lQuoted = quoted($1);
      bool rQuoted = quoted($3);
    
      if(lQuoted && rQuoted) $$ = strdup(concat($1, $3).c_str());
      else if(!lQuoted && !rQuoted) $$ = strdup(sum($1, $3).c_str());
      else $$ = strdup("@");
    } 
    free($1);
    free($3);
  }
;

operand:
  TK_ID  { 
    Var var = lookup($1);
    if(var.id.empty()) var = lookupVarList(vars, $1);
    if(var.id.empty()) std::cerr << "Erro: Identificador '"<< $1 <<"' nao foi declarado." << std::endl;
    else $$ = strdup(var.val.c_str()); 
    free($1);
  }
  | TK_NUMBER                   { $$ = strdup($1); free($1);}
  | TK_STRING                   { $$ = strdup($1); free($1);}
;

%%

