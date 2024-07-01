%{
  #include <iostream>
  #include <cstring>
  #include "interp.h"

  extern int yylex();
  extern int yyparse();
  extern FILE *yyin;
 
  void yyerror(const char *s);
  std::list<Var> vars;  
  constexpr const char* ERR_INVALID_ID() { return "eii"; }
  constexpr const char* ERR_DATA_TYPE()  { return "edt"; }
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
    
    /*
      Verifica se um identificador é válido. 
      Se for, busca o identificador `$1`. 
      Se encontrado, verifica se o tipo de dado coincide com o da expressão `$3`. 
      Se coincidir, faz a atribuição; caso contrário, exibe um erro de tipo incompatível. 
      Se o identificador não for encontrado, exibe um erro de declaração. 
      Por fim, libera a memória alocada.
    */

    if(strcmp($3, ERR_INVALID_ID()) != 0) {
      Var l = lookup($1);
      if(!l.id.empty()) {
        if(l.type == expressionType($3)) assign($1, $3);
        else std::cerr << "Erro: Tipo de dado incompativel entre identificador e expressao." << std::endl;
      }else
        std::cerr << "Erro: Identificador '" << $1 << "' nao foi declarado." << std::endl; 
    } 
      
    free($1); 
    free($3);
  }
;

decl_assig:
  TK_ID                         { vars.push_back(Var("", $1, "")); }
  | TK_ID EQ expr               { 
    
    /*
      Verifica se um identificador é válido e se não há erro de tipo de dado. 
      Se ambos os testes forem positivos, adiciona a variável à lista vars. 
      Caso contrário, exibe uma mensagem de erro de tipo de dado incompatível. 
      Por fim, libera a memória alocada para os identificadores.
    */

    if(strcmp($3, ERR_INVALID_ID()) != 0) {
      if(strcmp($3, ERR_DATA_TYPE()) != 0)
        vars.push_back(Var(expressionType($3), $1, $3));  
      else 
        std::cerr << "Erro: Tipo de dado incompativel entre operadores da expressao." << std::endl;
    }  
    free($1); 
    free($3);
  }
;

expr:
  operand                         { $$ = strdup($1); free($1); }
  | operand PLUS operand          {
    
    /*
      Verifica os dois operandos e, se qualquer um deles for inválido, retorna uma mensagem de erro. 
      Caso contrário, ele determina o tipo deles, e com base nisso, concatena ou soma os valores. 
      Se houver incompatibilidade de tipos, retorna um erro de tipo de dado. 
      Por fim, libera a memória alocada para os identificadores.
    */

    if(strcmp($1, ERR_INVALID_ID()) == 0 || strcmp($3, ERR_INVALID_ID()) == 0) 
      $$ = strdup(ERR_INVALID_ID());
    else {
      bool lQuoted = quoted($1);
      bool rQuoted = quoted($3);
    
      if(lQuoted && rQuoted) $$ = strdup(concat($1, $3).c_str());
      else if(!lQuoted && !rQuoted) $$ = strdup(sum($1, $3).c_str());
      else $$ = strdup(ERR_DATA_TYPE());
    } 
     
    free($1);
    free($3);
  }
;

operand:
  TK_ID  { 

    /*
      Rotina para buscar um identificador. 
      Primeiro, ele tenta localizar o identificador usando a função lookup, que procura na pilha de escopos do programa. 
      Se não encontrar, tenta novamente com lookupVarList, que procura na lista temporaria para declaracoes. 
      Se ainda assim o identificador não for encontrado, define uma mensagem de erro e exibe no console que o identificador não foi declarado. 
      Caso contrário, atribui o valor associado ao identificador encontrado. 
      Por fim, libera a memória alocada para o identificador.
    */

    Var var = lookup($1);
    if(var.id.empty()) var = lookupVarList(vars, $1);
    if(var.id.empty()) {
      $$ = strdup(ERR_INVALID_ID());
      std::cerr << "Erro: Identificador '"<< $1 <<"' nao foi declarado." << std::endl;
    } 
    else $$ = strdup(var.val.c_str()); 
    free($1);
  }
  | TK_NUMBER                   { $$ = strdup($1); free($1);}
  | TK_STRING                   { $$ = strdup($1); free($1);}
;

%%

