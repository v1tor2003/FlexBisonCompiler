#include "interp.h"
#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>

std::stack<Scope> scopeStack;


void enterScope() {scopeStack.push(Scope()); }
void exitScope() { if(!scopeStack.empty()) scopeStack.pop(); }

void print(const std::string& id){ 
  Var v = lookup(id);
  if(!v.id.empty()){
    std::string str = v.val;
    if(str.empty() && v.type == "CADEIA") str = "\"\"";
    std::cout << str << std::endl;
  } else
  std::cerr << "Error: Identifier '"<<id <<"' was not defined." << std::endl;
}

void define(Var v){
  if(scopeStack.empty()) throw std::runtime_error("No declared scopes.");
  auto& currScope = scopeStack.top();
  currScope.symbols[v.id] = v;
}

void assign(const std::string& id, const std::string& newVal){
  if(scopeStack.empty()) throw std::runtime_error("No declared scopes.");
  auto& currScope = scopeStack.top();
  currScope.symbols[id].val = newVal;
}

void defineMany(const std::string& type, std::list<Var>& vars){
  for(Var& v : vars) {
    if(v.type.empty()) v.type = type;
    if(v.type != type) {
      std::cout << "Error: Incompatible data type for identifier '" << v.id << "'."<<" Expected: " << type <<", got: " << v.type << std::endl;
      continue;
    }
    if(v.val.empty() && v.type == "NUMERO") v.val = "0";
    define(v);
  }
  vars.clear();
}

Var lookupVarList(const std::list<Var>& vars, const std::string& id) {
  auto it = std::find_if(vars.begin(), vars.end(), [&id](const Var& var) {
    return var.id == id;
  });

  if (it != vars.end()) return (*it);
  else return Var ("", "", "");
}

Var lookup(const std::string& id){
  std::stack<Scope> temp = scopeStack;
  while (!temp.empty())
  {
    auto& currScope = temp.top();
    if(currScope.symbols.find(id) != currScope.symbols.end())
      return currScope.symbols[id];
      
    temp.pop();
  }
  
  return Var ("", "", "");
}

bool quoted(const std::string& str){
  size_t len = str.length();
  char quote = '"';
  return (len >= 2 && str[0] == '"' && str[len-1] == quote);
}

std::string sum(const std::string& v1, const std::string& v2){
  float res = std::stof(v1) + std::stof(v2);
  return std::to_string(res);
}

std::string concat(std::string str1, std::string str2){
  return (str1.erase(str1.length() - 1) + str2.erase(0, 1));
}

std::string expressionType(const std::string& expr){
  if(quoted(expr)) return "CADEIA";
  else return "NUMERO";
}