#include "interp.h"
#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>

// Pilha global de scopos do programa
std::stack<Scope> scopeStack;

/** 
  * Funcao para entrar em um escopo com nomeado com name
  * @param {string} name
  * @returns {void} procedimento, sem retorno  
*/
void enterScope(const std::string& name) {
  scopeStack.push(Scope()); 
  scopeStack.top().name = name;
}

/** 
  * Funcao para uma sair de um escopo nomeado com name
  * @param {string} name
  * @returns {void} procedimento, sem retorno  
*/
void exitScope(const std::string& name) { 
  if(!scopeStack.empty() && scopeStack.top().name == name) { 
    scopeStack.pop();
    return;
  }

  std::cerr <<"Erro: O nome do escopo atual nao corresponde ao comando." << std::endl;
}

/** 
  * Funcao para uma dada variavel, acha na pilha de escopos do programa e mostrar seu valor na saida padrao
  * @param {const ref string} id
  * @returns {void} procedimento, sem retorno  
*/
void print(const std::string& id){ 
  Var var = lookup(id);
  if(!var.id.empty()){
    std::string str = var.val;
    if(var.type == "CADEIA") str = formatString(var.val);
    else str = formatFromReal(var.val);
    std::cout << str << std::endl;
  } else
    std::cerr << "Erro: Indentificador '"<< id <<"' nao foi declarado." << std::endl;
}

/** 
  * Funcao para uma dada variavel, defini-la na tabela do escopo atual
  * @param {Var} v
  * @returns {void} procedimento, sem retorno  
*/
void define(Var v){
  if(scopeStack.empty()) throw std::runtime_error("Sem escopos declarados.");
  auto& currScope = scopeStack.top();
  currScope.symbols[v.id] = v;
}

/** 
  * Funcao para uma dada variavel, e valor achar-la no programa e atualizar seu valor
  * @param {const ref string, const ref string, ref stack<Scope>} v
  * @returns {void} procedimento, sem retorno  
*/
bool assignHelper(const std::string& id, const std::string& newVal,std::stack<Scope>& scopes){
  std::stack<Scope> temp;

  while (!scopes.empty())
  {
    auto& currScope = scopes.top();
    auto it = currScope.symbols.find(id);
    if(it != currScope.symbols.end()) {
      currScope.symbols[id].val = newVal;
      stackCopy(temp, scopes);
      return true;
    }
    temp.push(scopes.top());
    scopes.pop();
  }

  stackCopy(temp, scopes);
 
  return false;
}

/** 
  * Funcao para copiar em memoria elementos de um pilha origem para outra destino
  * @param {ref stack<Scope>, ref stack<Scope>} source, dest
  * @returns {void} procedimento, sem retorno  
*/
void stackCopy(std::stack<Scope>& source, std::stack<Scope>& dest){
  while (!source.empty())
  {
    dest.push(source.top());
    source.pop();
  }
}

/** 
  * Funcao para atribuir um novo valor a uma dada variavel
  * @param {const ref string, const ref string} type, vars 
  * @returns {void} procedimento, sem retorno  
*/
void assign(const std::string& id, const std::string& newVal){
  if(scopeStack.empty()) throw std::runtime_error("Sem escopos declarados.");
 
  if(!assignHelper(id, newVal, scopeStack))
    std::cerr << "Erro: Identificador '" << id << "' nao foi declarado." << std::endl;  

}

/** 
  * Funcao para definir cada variavel presente na lista de variaveis 
  * alem de atribuir o tipo dependendo da forma que a variavel foi gerada pelo parser
  * @param {const ref string, ref list<Var>} type, vars 
  * @returns {void} procedimento, sem retorno  
*/
void defineMany(const std::string& type, std::list<Var>& vars){
  for(Var& v : vars) {
    if(v.type.empty()) v.type = type;
    if(v.type != type) {
      std::cout << "Erro: Tipo de dado incompativel para identificador '" << v.id << "'."<<" Esperado: " << type <<", recebido: " << v.type << std::endl;
      continue;
    }
    if(v.val.empty() && v.type == "NUMERO") v.val = "0";
    define(v);
  }
  vars.clear();
}

/** 
  * Funcao para achar uma variavel na lista temporaria de variaveis 
  * auxilia na definicao de variaveis pois no momento da declarao uma variavel pode depender
  * de outra que ainda esta para ser definida
  * @param {const ref list<Var>, const ref string} vars, id 
  * @returns {Var} Var(id, type, val)  
*/
Var lookupVarList(const std::list<Var>& vars, const std::string& id) {
  auto it = std::find_if(vars.begin(), vars.end(), [&id](const Var& var) {
    return var.id == id;
  });

  if (it != vars.end()) return (*it);
  else return Var ();
}

/** 
  * Funcao para achar uma variavel na tabela dos escopos da pilha
  * se nao achar na tabela do escopo atual, procura em seus antecessores ate que nao exista mais antecessores
  * @param {const ref string} id 
  * @returns {Var} Var(id, type, val)  
*/
Var lookup(const std::string& id){
  std::stack<Scope> temp = scopeStack;
  while (!temp.empty())
  {
    auto& currScope = temp.top();
    if(currScope.symbols.find(id) != currScope.symbols.end())
      return currScope.symbols[id];
      
    temp.pop();
  }
  
  return Var ();
}

/** 
  * Funcao para uma dada string verifca se ela comeca e termina com "
  * @param {const ref string} str
  * @returns {void} procedimento, sem retorno
*/
bool quoted(const std::string& str){
  size_t len = str.length();
  char quote = '"';
  return (len >= 2 && str[0] == '"' && str[len-1] == quote);
}

/** 
  * Funcao que para dois numeros dados em string perfoma a operacao de some e retorno a representacao em string
  * @param {const ref string, const ref string} num1, num2
  * @returns {string} retona string (num1 + num2)
*/
std::string sum(const std::string& v1, const std::string& v2){
  float res = std::stof(v1) + std::stof(v2);
  return std::to_string(res);
}

/** 
  * Funcao que para duas strings perfoma a operacao de concatenacao
  * @param {copy string, copy string} str1, str2
  * @returns {string} retona str1 + str1
*/
std::string concat(std::string str1, std::string str2){
  return (str1.erase(str1.length() - 1) + str2.erase(0, 1));
}

/** 
  * Funcao para verificar o tipo de uma expressao
  * @param {const ref string} expr
  * @returns {string} "NUMERO" | "CADEIA"
*/
std::string expressionType(const std::string& expr){
  if(quoted(expr)) return "CADEIA";
  else return "NUMERO";
}

/** 
  * Funcao para dado um numero real em string formatar seus 0s
  * @param {const ref string} expr
  * @returns {string} string
*/
std::string formatFromReal(const std::string& number){
  size_t dotPos = number.find('.');
  if(dotPos == std::string::npos) return number;
  size_t endPos = number.find_last_not_of('0');
  if(endPos == dotPos) return number.substr(0, dotPos);

  return number.substr(0, endPos + 1);
}

/** 
  * Funcao para formatar strings, se a string for vazia adiciona " nas extremidades
  * @param {const ref string} expr
  * @returns {string} NUMERO | CADEIA
*/
std::string formatString(const std::string& str) {
  return str.empty() ? '"' + str + '"' : str;
}