/*
 * File:	type.cpp
 *
 * Description:	Implements type class
 */


#include <cassert>
#include "type.h"
#include "iostream"

using namespace std;

bool Type::operator==(const Type &that) const {

    if(_kind != that._kind)
        return false;

    //if(_kind == ERROR)
    //    return true;

    if(_specifier != that._specifier || _indirection != that._indirection)
        return false;

    if(_kind == SCALAR)  
        return true;
    
    if (_kind == ARRAY)
        return _length == that._length;

    assert(_kind == FUNCTION);
    if(_parameters->variadic != that._parameters->variadic)
        return false;

    return _parameters->types == that._parameters->types;
}


bool Type::operator!=(const Type &type) const {

    return !operator==(type);
}


ostream& operator<<(std::ostream& ostr, const Type& type){
    return ostr << "specifier: " << type._specifier << ", indir: " << type._indirection << ", kind: " << type._kind << endl;
}





Type::Type(int specifier, unsigned indirection = 0){
    this->_specifier = specifier;
    this->_indirection = indirection;
    this->_kind = SCALAR;
}

Type::Type(int specifier, unsigned indirection, unsigned length){
    this->_specifier = specifier;
    this->_indirection = indirection;
    this->_kind = ARRAY;
    this->_length = length;
}

Type::Type(int specifier, unsigned indirection, Parameters* parameters){
    this->_specifier = specifier;
    this->_indirection = indirection;
    this->_kind = FUNCTION;   
    this->_parameters = parameters;
}


Type::Type(){
    cout << "default type constructor called, error" << endl;
    exit(EXIT_FAILURE);
}


