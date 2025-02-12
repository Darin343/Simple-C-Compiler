#include "scope.h"
#include <cassert>




void Scope::printSymbols(){
    
    std::cout << "syms: " << std::endl;

    for(int i = 0; i < (int)_symbols.size(); i++){
        std::cout << _symbols[i]->name() << ", "; 
    }
    std::cout << std::endl;
    return;
}




Symbol* Scope::find(const std::string &name) const {
    
    std::cout << "finding symbol: " << name << std::endl;

    for(int i = 0; i < (int)_symbols.size(); i++){
        if(_symbols[i]->name() == name){
            return _symbols[i];
            std::cout << "found symbol: " << name << std::endl;
        }
    }

    return nullptr;
}


Symbol* Scope::lookup(const std::string &name) const {




    Symbol* currentScopeFind = find(name); 

    if(currentScopeFind != nullptr)
        return currentScopeFind;

    if(_enclosing == nullptr)
        return nullptr;

    return _enclosing->lookup(name);


    /*
    const Scope* next = this;
    assert(next != nullptr);
    std::cerr << "looking up symbol: " << name << std::endl;

    Symbol* currentScopeFind;
    while(1){
        


        currentScopeFind = next->find(name); 

        if (currentScopeFind == nullptr){
            
            if (next->_enclosing == nullptr) {
                
                return nullptr;
            } 

            std::cerr << "trying deeper scope " << this->_enclosing << std::endl;
            
            next = this->_enclosing;   
            

        } else {
            std::cerr << "lookup found symbol " << name << std::endl;
            return currentScopeFind;
        }
        
    }
    */
}

void Scope::insert(Symbol* sym){
    _symbols.push_back(sym);
}


Scope::Scope(Scope* enclosing){
    _enclosing = enclosing;

} //default enclosing scope == nullptr unless otherwise specified


