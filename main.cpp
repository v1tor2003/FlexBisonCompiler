#include <iostream>
#include <stack>
#include "parser.tab.h"

extern int yyparse();
extern FILE *yyin;
// Ponto de entrada para execucao do programa, para chamadas ao parser
int main (int argc, char **argv){
  if(argc < 2) throw std::runtime_error("Arquivo de entrada nao especificado.");

  FILE *in = fopen(argv[1], "r");
  if(!in) throw std::runtime_error("Erro ao abrir arquivo.");
  
  yyin = in;
  do { yyparse(); } while (!feof(yyin));
  fclose(yyin);  

  return 0;
}

void yyerror (const char *s){
  std::cerr << "Parser Error: " << s << std::endl;
}

