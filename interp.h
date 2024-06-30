#ifndef INTERP_H
#define INTERP_H

#include <iostream>
#include <unordered_map>
#include <stack>
#include <list>

struct Var
{
  std::string type;
  std::string id;
  std::string val;

  Var(const std::string& type = "", const std::string& id = "", const std::string& val = "") 
    : type(type), id(id), val(val) {}
};

using SymbolTable = std::unordered_map<std::string, Var>;

struct Scope 
{
  std::string name;
  SymbolTable symbols;
};

void stackCopy(std::stack<Scope>& source, std::stack<Scope>& dest);
void enterScope(const std::string& name);
void exitScope(const std::string& name);
void print(const std::string& id);
void define(Var v);
void defineMany(const std::string& type, std::list<Var>& vars);
void assign(const std::string& id, const std::string& newVal);
bool assignHelper(const std::string& id, const std::string& newVal, std::stack<Scope>& scopes);
bool quoted (const std::string& str);
Var lookup(const std::string& id);
Var lookupVarList(const std::list<Var>& vars, const std::string& id);
std::string concat(std::string str1, std::string str2);
std::string sum(const std::string& str1, const std::string& str2);
std::string expressionType(const std::string& expr);
std::string formatFromReal(const std::string& number);
std::string formatString(const std::string& str);
#endif // INTERP_H