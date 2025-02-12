/*
 * File:	type.h
 *
 * Description:	Outlines type class
 */

#ifndef TYPE_H
#define TYPE_H
#include <vector>
#include <iostream> 

enum { SCALAR, ARRAY, FUNCTION };

typedef std::vector<class Type> Types;

struct Parameters {
    bool variadic;
    Types types;
};




class Type {

    int _specifier;
    unsigned _indirection;
    int _kind;


    unsigned long _length;
    Parameters* _parameters;


    public:

    //constructors
    Type(int specifier, unsigned indirection);
    Type(int specifier, unsigned indirection, unsigned length);
    Type(int specifier, unsigned indirection, Parameters* parameters);
    Type();



    //accessors
    int kind() const {
        return _kind;
    }

    int specifier() const {
        return _specifier;
    }

    unsigned indirection() const {
        return _indirection;
    }


    unsigned long length() const;
    Parameters* parameters() const; 

    bool operator==(const Type &that) const;
    bool operator!=(const Type &that) const;

    
    friend std::ostream& operator<<(std::ostream& ostr, const Type& type);

};


#endif