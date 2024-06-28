#include <iostream>
#include <stack>
#include "parser.tab.h"

extern int yyparse();
extern FILE *yyin;

int main (int argc, char **argv){
  if(argc < 2) throw std::runtime_error("No input file was provided.");

  FILE *in = fopen(argv[1], "r");
  if(!in) throw std::runtime_error("Could not open input file.");
  
  yyin = in;
  do { yyparse(); } while (!feof(yyin));
  fclose(yyin);  

  return 0;
}

void yyerror (const char *s){
  std::cerr << "Parser Error: " << s << std::endl;
}

