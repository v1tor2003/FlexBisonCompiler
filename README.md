# Projeto Flex e Bison em C++

Este é um projeto que faz o uso das ferramentas Flex e Bison em um projeto C++ para análise léxica e sintática de uma linguagem simples, com foco em gerenciamento de escopos e variaveis.

## Requisitos

Para compilar e executar este projeto, você precisará ter instalado:

- **Flex (lex)**: Ferramenta para geração de analisadores léxicos.
- **Bison (yacc)**: Ferramenta para geração de analisadores sintáticos.
- **C++ Compiler (g++)**: Um compilador C++ compatível com o padrão C++11 ou superior.

## Estrutura do Projeto

- `lexer.l`: Arquivo contendo as regras léxicas escritas em Flex.
- `parser.y`: Arquivo contendo as regras gramaticais escritas em Bison.
- `interp.h`: Arquivo com as declarações dos procedimentos para interpretar a linguagem
- `interp.cpp`: Arquivo com as implementações do interpretador
- `main.cpp`: Arquivo principal onde o analisador léxico e sintático é utilizado.
- `README.md`: Este arquivo que você está lendo agora.

## Compilação e Execução

Para compilar o projeto, utilize os seguintes comandos:

```bash
bison -d parser.y
flex lexer.l
g++ -std=c++11 main.cpp interp.cpp parser.tab.c lex.yy.c -lfl -o interp
