/*
 * File:	checker.cpp
 *
 * Description:	Not sure
 *		
 */


#include "checker.h"
#include <iostream>
#include <set>
#include "type.h"
#include "scope.h"
#include "lexer.h"

using namespace std; //needed for endl?

Scope* current = nullptr; //scope the program currently has open
Scope* globalScope = nullptr;
std::vector<std::string> definedFunctions;

const std::string E1 = "redefinition of '%s'"; 
const std::string E2 = "conflicting types for '%s'";
const std::string E3 = "redeclaration of '%s'";
const std::string E4 = "'%s' undeclared";

void checkIdentifier(const std::string &name){

    Symbol* oldDec = current->lookup(name);
    if (oldDec == nullptr){
        
        //name undeclared
        report(E4, name);
        return;
    }
    std::cout << "checked identifier " << name << std::endl;
}


//helper function bc i couldnt find how to search thru a vector contains doesnt work
bool definedFunctionsContains(std::string target){

    for(std::string i : definedFunctions) {
        if(i == target)
            return true;
    }
    return false;
}





void openScope(){

    if(current == nullptr){ //open global scope
        std::cout << "open global scope" << std::endl;

        Scope* newScope = new Scope(nullptr); 
        newScope->setGlobalScope();
                
        current = newScope; 
        globalScope = newScope;

        return;
    } else { //open local scope
        std::cout << "open local scope" << std::endl;

        Scope* newScope = new Scope(current); //new scope's _enclosing set to current
        current = newScope;       //update current scope to newly made scope

        return;
    }



}




void closeScope(){

    //std::cout << "closing scope:" << current << std::endl;

    Scope* temp = current; 
    current = current->enclosing(); 

    delete temp;

    std::cout << "close scope" << std::endl;
}





void declareFunction(const std::string &name, const Type &type){

    Symbol* oldDec = globalScope->find(name);
    
    if (oldDec == nullptr){ //symbol does not exist

        Symbol* newDef = new Symbol(name, type);
        globalScope->insert(newDef); 
        //cerr << "added to defined fns " << name << endl;

        std::cout << "declare function: " << name << ", " << type << std::endl;
        return; 

    } else { //symbol exists

        if(oldDec->type() != type){

            //conflicting types
            report(E2, name);


        } else {
            cout << "function declared alr but its cool" << endl;
            //symbol alr exists in table, name therefore alr exists in set, types match no need to do anything
            return;
        }

        


    }
}


void defineFunction(const std::string &name, const Type &type){
    
    if(definedFunctionsContains(name)){ //helper function
        

        report(E1, name);   //redfinition of name
        return;
 
        /*
        Symbol* oldDef = globalScope->find(name);
        if(oldDef->type() != type) {
            
            report(E1, name);   //redfinition of name
            return;

        } else {
            report(E1, name);
            return;
        }*/

    } else { //name not in set        
        
        definedFunctions.push_back(name);  
        declareFunction(name, type);
        return;
    }     
}





//good
void declareVariable(const std::string &name, const Type &type){


    if(current == globalScope){                                     //if in global scope

        //see if name exists
        Symbol* oldDec = globalScope->find(name);                   //only need to check global scope, no enclosing
        

        if(oldDec == nullptr) {                                     //find() fail, symbol does not exist and we must insert later
            Symbol* newVar = new Symbol(name, type);
            globalScope->insert(newVar); 
            std::cout << "declare variable: " << name << ", type info: " << type << std::endl;
            return;


        } else {                                                    //find() success, must check taht types match.
            if(oldDec->type() == type){
                //types match for variable in global dec, its ok
                return;
            } else {
                //types do not match, conflicting types
                report(E2, name);
                return;
            }
        }



    } else {                                                          //if in local scope

        //see if name exists in local scope first
        Symbol* currentScopeDecl = current->find(name);

        if(currentScopeDecl == nullptr) { //not in local scope, declare
            
            Symbol* newDef = new Symbol(name, type);
            current->insert(newDef);
            current->printSymbols();

            std::cout << "declare variable: " << name << ", type info: " << type << std::endl;

        } else { //varable is already declared in local scope

            report(E3, name);
            return;
        }

    }
    
}