#ifndef CHECKER_H
#define CHECKER_H

#include <string>
#include "type.h"

void checkIdentifier(const std::string &name);

void openScope();
void closeScope();

void defineFunction(const std::string &name, const Type &type);
void declareFunction(const std::string &name, const Type &type);
void declareVariable(const std::string &name, const Type &type);



#endif

