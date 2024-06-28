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
  | r_assig ENDL                
  | BLOCK TK_ID_BLOCK           { enterScope();        }
  | END_BLOCK                   { exitScope();         }
  | PRINT TK_ID ENDL            { print($2); free($2); }
; 

decl:
  TYPE list_decls               { defineMany($1, vars); free($1)}
;

list_decls:
  decl_assig                    
  | list_decls COMMA decl_assig 
;
r_assig:
  TK_ID EQ expr               {
    if($3[0] != '@') {
      Var l = lookup($1);
      if(!l.id.empty()) {
        if(l.type == expressionType($3)) assign($1, $3);
        else std::cerr << "Error: Identifier and expression types must match." << std::endl;
      }else
      std::cerr << "Error: Identifier '" << $1 << "' was not defined." << std::endl;
      free($1); 
      free($3);
    } else 
    std::cerr << "Error: Assignment expression operands types must match." << std::endl;
  }
;
decl_assig:
  TK_ID EQ expr               { 
    if($3[0] != '@') {
      std::string t;
      if($3[0] == '"') t = "CADEIA";
      else t = "NUMERO";

      vars.push_back(Var(t, $1, $3));
      
      free($1); 
      free($3);
    } else 
    std::cerr << "Error: Declaration expression operands types must match." << std::endl;
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
      else $$ = "@";
    } 
  }
;

operand:
  TK_ID  { 
    Var var = lookup($1);
    if(var.id.empty()) var = lookupVarList(vars, $1);
    if(var.id.empty()) std::cerr << "Error: Identifier '"<< $1 <<"' was not defined." << std::endl;
    else $$ = strdup(var.val.c_str()); 
    free($1);
  }
  | TK_NUMBER                   { $$ = strdup($1); free($1);}
  | TK_STRING                   { $$ = strdup($1); free($1);}
;

%%

