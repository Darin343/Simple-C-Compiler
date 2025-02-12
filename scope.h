#ifndef SCOPE_H
#define SCOPE_H
#include "symbol.h"
#include <string>
#include <vector> //scope = vector of symbols

typedef std::vector<Symbol*> Symbols;

class Scope {

    Scope* _enclosing;
    Symbols _symbols;


    public:

    //constructor
    Scope(Scope* enclosing = 0); //default enclosing scope == nullptr unless otherwise specified
    

    void insert(Symbol* sym);
    
    void setGlobalScope(){
        _enclosing = nullptr;
    }

    Scope* enclosing() const{
        return _enclosing;
    }

    const Symbols& symbols() const{
        return _symbols;
    }


    Symbol* find(const std::string &name) const;
    Symbol* lookup(const std::string &name) const;

    void printSymbols();
};


#endif