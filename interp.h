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

  Var() : id(""), val(""), type("") {}
  Var(const std::string& type, const std::string& id, const std::string& val) : type(type), id(id), val(val) {}
};

using SymbolTable = std::unordered_map<std::string, Var>;

struct Scope 
{
  SymbolTable symbols;
};

void print(const std::string& id);
void enterScope();
void exitScope();
void define(Var v);
void assign(const std::string& id, const std::string& newVal);
void defineMany(const std::string& type, std::list<Var>& vars);
Var lookup(const std::string& id);
bool quoted (const std::string& str);
std::string concat(std::string str1, std::string str2);
std::string sum(const std::string& str1, const std::string& str2);
Var lookupVarList(const std::list<Var>& vars, const std::string& id);
std::string expressionType(const std::string& expr);
#endif // INTERP_H