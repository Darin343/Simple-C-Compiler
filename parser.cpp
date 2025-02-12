/*
 * File:	parser.cpp
 *
 * Description:	This file contains the public and private function and
 *		variable definitions for the recursive-descent parser for
 *		Simple C.
 */

# include <cstdlib>
# include <iostream>
# include "tokens.h"
# include "lexer.h"
# include "checker.h"
# include "type.h"
# include "type.cpp"

#include "symbol.h"
#include "scope.h"

#include <string>

using namespace std;

static int lookahead, nexttoken;
static string lexbuf, nextbuf;

static void expression();
static void statement();
static void match(int t);


//-------------------------------------

static string identifier(){

	string buf = lexbuf;
	match(ID);
	return buf;

}

static string num(){

	//lexbuf string to int return int
	return 0;

}


//------------------------------------




/*
 * Function:	error
 *
 * Description:	Report a syntax error to standard error.
 */

static void error()
{
    if (lookahead == DONE)
	report("syntax error at end of file");
    else
	report("syntax error at '%s'", lexbuf);

    exit(EXIT_FAILURE);
}


/*
 * Function:	match
 *
 * Description:	Match the next token against the specified token.  A
 *		failure indicates a syntax error and will terminate the
 *		program since our parser does not do error recovery.
 */

static void match(int t)
{
    if (lookahead != t)
	error();

    if (nexttoken != 0) {
	lookahead = nexttoken;
	lexbuf = nextbuf;
	nexttoken = 0;
    } else {
	lookahead = yylex();
	lexbuf = yytext;
    }
}


/*
 * Function:	peek
 *
 * Description:	Return the next token in the input stream and save it so
 *		that match() will later return it.
 */

static int peek()
{
    if (!nexttoken) {
	nexttoken = yylex();
	nextbuf = yytext;
    }

    return nexttoken;
}


/*
 * Function:	isSpecifier
 *
 * Description:	Return whether the given token is a type specifier.  Simple
 *		C has only char, int, and long.
 */

static bool isSpecifier(int token)
{
    return token == CHAR || token == INT || token == LONG;
}


/*
 * Function:	specifier
 *
 * Description:	Parse a type specifier.
 *
 *		specifier:
 *		  char
 *		  int
 *		  long
 */

int specifier()
{
	int typespec = lookahead;


    if (isSpecifier(lookahead)){
		match(lookahead);
	
	} else {

		error();
	}

	return typespec;
}


/*
 * Function:	pointers
 *
 * Description:	Parse pointer declarators (i.e., zero or more asterisks).
 *
 *		pointers:
 *		  empty
 *		  * pointers
 */

unsigned pointers()
{
	unsigned count = 0;

    while (lookahead == '*'){

		match('*');
		count++;
	}
	return count;
}


/*
 * Function:	declarator
 *
 * Description:	Parse a declarator, which in Simple C is either a scalar
 *		variable or an array, with optional pointer declarators.
 *
 *		declarator:
 *		  pointers identifier
 *		  pointers identifier [ num ]
 */

static void declarator(int typespec) {

	unsigned indirection = pointers(); 
	std::string nameID = lexbuf;
    match(ID);

    if (lookahead == '[') {

		match('[');
		unsigned long arrayLength = std::stoul(lexbuf, nullptr, 0);
		match(NUM);
		match(']');


		Type varType(typespec, indirection, arrayLength); 

		declareVariable(nameID, varType);
		//cout << "(" << typespec << ", " << indirection << ", ARRAY)" << endl;


    } else {

		Type varType(typespec, indirection); 

		declareVariable(nameID, varType);
		//cout << "(" << typespec << ", " << indirection << ", SCALAR)" << endl;
	}
}


/*
 * Function:	declaration
 *
 * Description:	Parse a local variable declaration.  Global declarations
 *		are handled separately since we need to detect a function
 *		as a special case.
 *
 *		declaration:
 *		  specifier declarator-list ';'
 *
 *		declarator-list:
 *		  declarator
 *		  declarator , declarator-list
 */

static void declaration()
{
	//old:
    //specifier();
    //declarator();

	//rewriting as:
	int typespec = specifier();
	declarator(typespec);


    while (lookahead == ',') {
	match(',');
	declarator(typespec);
    }

    match(';');
}


/*
 * Function:	declarations
 *
 * Description:	Parse a possibly empty sequence of declarations.
 *
 *		declarations:
 *		  empty
 *		  declaration declarations
 */

static void declarations()
{
    while (isSpecifier(lookahead))
	declaration();
}


/*
 * Function:	primaryExpression
 *
 * Description:	Parse a primary expression.
 *
 *		primary-expression:
 *		  ( expression )
 *		  identifier ( expression-list )
 *		  identifier ( )
 *		  identifier
 *		  num
 *		  character
 *		  string
 *
 *		expression-list:
 *		  expression
 *		  expression , expression-list
 */

static void primaryExpression()
{
    if (lookahead == '(') {
	match('(');
	expression();
	match(')');

    } else if (lookahead == STRING) {
	match(STRING);

    } else if (lookahead == CHARACTER) {
	match(CHARACTER);

    } else if (lookahead == NUM) {
	match(NUM);

    } else if (lookahead == ID) {
	
	checkIdentifier(identifier()); 		//handles match(ID) in identifier
										//identifier returns lexbuf string to checkIdentifier
	
	if (lookahead == '(') {
	    match('(');

	    if (lookahead != ')') {
		expression();

		while (lookahead == ',') {
		    match(',');
		    expression();
		}
	    }

	    match(')');
	}

    } else
	error();
}


/*
 * Function:	postfixExpression
 *
 * Description:	Parse a postfix expression.
 *
 *		postfix-expression:
 *		  primary-expression
 *		  postfix-expression [ expression ]
 */

static void postfixExpression()
{
    primaryExpression();

    while (lookahead == '[') {
	match('[');
	expression();
	match(']');
	cout << "index" << endl;
    }
}


/*
 * Function:	prefixExpression
 *
 * Description:	Parse a prefix expression.
 *
 *		prefix-expression:
 *		  postfix-expression
 *		  ! prefix-expression
 *		  - prefix-expression
 *		  * prefix-expression
 *		  & prefix-expression
 *		  sizeof prefix-expression
 *		  sizeof ( specifier pointers )
 *		  ( specifier pointers ) prefix-expression
 *
 *		This grammar is still ambiguous since "sizeof(type) * n"
 *		could be interpreted as a multiplicative expression or as a
 *		cast of a dereference.  The correct interpretation is the
 *		former, as the latter makes little sense semantically.  We
 *		resolve the ambiguity here by always consuming the "(type)"
 *		as part of the sizeof expression.
 */

static void prefixExpression()
{
    if (lookahead == '!') {
	match('!');
	prefixExpression();
	cout << "not" << endl;

    } else if (lookahead == '-') {
	match('-');
	prefixExpression();
	cout << "neg" << endl;

    } else if (lookahead == '*') {
	match('*');
	prefixExpression();
	cout << "deref" << endl;

    } else if (lookahead == '&') {
	match('&');
	prefixExpression();
	cout << "addr" << endl;

    } else if (lookahead == SIZEOF) {
	match(SIZEOF);

	if (lookahead == '(' && isSpecifier(peek())) {
	    match('(');
	    specifier();
	    pointers();
	    match(')');
	} else
	    prefixExpression();

	cout << "sizeof" << endl;

    } else if (lookahead == '(' && isSpecifier(peek())) {
	match('(');
	specifier();
	pointers();
	match(')');
	prefixExpression();
	cout << "cast" << endl;

    } else
	postfixExpression();
}


/*
 * Function:	multiplicativeExpression
 *
 * Description:	Parse a multiplicative expression.
 *
 *		multiplicative-expression:
 *		  prefix-expression
 *		  multiplicative-expression * prefix-expression
 *		  multiplicative-expression / prefix-expression
 *		  multiplicative-expression % prefix-expression
 */

static void multiplicativeExpression()
{
    prefixExpression();

    while (1) {
	if (lookahead == '*') {
	    match('*');
	    prefixExpression();
	    cout << "mul" << endl;

	} else if (lookahead == '/') {
	    match('/');
	    prefixExpression();
	    cout << "div" << endl;

	} else if (lookahead == '%') {
	    match('%');
	    prefixExpression();
	    cout << "rem" << endl;

	} else
	    break;
    }
}


/*
 * Function:	additiveExpression
 *
 * Description:	Parse an additive expression.
 *
 *		additive-expression:
 *		  multiplicative-expression
 *		  additive-expression + multiplicative-expression
 *		  additive-expression - multiplicative-expression
 */

static void additiveExpression()
{
    multiplicativeExpression();

    while (1) {
	if (lookahead == '+') {
	    match('+');
	    multiplicativeExpression();
	    cout << "add" << endl;

	} else if (lookahead == '-') {
	    match('-');
	    multiplicativeExpression();
	    cout << "sub" << endl;

	} else
	    break;
    }
}


/*
 * Function:	relationalExpression
 *
 * Description:	Parse a relational expression.  Note that Simple C does not
 *		have shift operators, so we go immediately to additive
 *		expressions.
 *
 *		relational-expression:
 *		  additive-expression
 *		  relational-expression < additive-expression
 *		  relational-expression > additive-expression
 *		  relational-expression <= additive-expression
 *		  relational-expression >= additive-expression
 */

static void relationalExpression()
{
    additiveExpression();

    while (1) {
	if (lookahead == '<') {
	    match('<');
	    additiveExpression();
	    cout << "ltn" << endl;

	} else if (lookahead == '>') {
	    match('>');
	    additiveExpression();
	    cout << "gtn" << endl;

	} else if (lookahead == LEQ) {
	    match(LEQ);
	    additiveExpression();
	    cout << "leq" << endl;

	} else if (lookahead == GEQ) {
	    match(GEQ);
	    additiveExpression();
	    cout << "geq" << endl;

	} else
	    break;
    }
}


/*
 * Function:	equalityExpression
 *
 * Description:	Parse an equality expression.
 *
 *		equality-expression:
 *		  relational-expression
 *		  equality-expression == relational-expression
 *		  equality-expression != relational-expression
 */

static void equalityExpression()
{
    relationalExpression();

    while (1) {
	if (lookahead == EQL) {
	    match(EQL);
	    relationalExpression();
	    cout << "eql" << endl;

	} else if (lookahead == NEQ) {
	    match(NEQ);
	    relationalExpression();
	    cout << "neq" << endl;

	} else
	    break;
    }
}


/*
 * Function:	logicalAndExpression
 *
 * Description:	Parse a logical-and expression.  Note that Simple C does
 *		not have bitwise-and expressions.
 *
 *		logical-and-expression:
 *		  equality-expression
 *		  logical-and-expression && equality-expression
 */

static void logicalAndExpression()
{
    equalityExpression();

    while (lookahead == AND) {
	match(AND);
	equalityExpression();
	cout << "and" << endl;
    }
}


/*
 * Function:	expression
 *
 * Description:	Parse an expression, or more specifically, a logical-or
 *		expression, since Simple C does not allow comma or
 *		assignment as an expression operator.
 *
 *		expression:
 *		  logical-and-expression
 *		  expression || logical-and-expression
 */

static void expression()
{
    logicalAndExpression();

    while (lookahead == OR) {
	match(OR);
	logicalAndExpression();
	cout << "or" << endl;
    }
}


/*
 * Function:	statements
 *
 * Description:	Parse a possibly empty sequence of statements.  Rather than
 *		checking if the next token starts a statement, we check if
 *		the next token ends the sequence, since a sequence of
 *		statements is always terminated by a closing brace.
 *
 *		statements:
 *		  empty
 *		  statement statements
 */

static void statements()
{
    while (lookahead != '}')
	statement();
}


/*
 * Function:	Assignment
 *
 * Description:	Parse an assignment statement.
 *
 *		assignment:
 *		  expression = expression
 *		  expression
 */

static void assignment()
{
    expression();

    if (lookahead == '=') {
	match('=');
	expression();
    }
}


/*
 * Function:	statement
 *
 * Description:	Parse a statement.  Note that Simple C has so few
 *		statements that we handle them all in this one function.
 *
 *		statement:
 *		  { declarations statements }
 *		  break ;
 *		  return expression ;
 *		  while ( expression ) statement
 *		  for ( assignment ; expression ; assignment ) statement
 *		  if ( expression ) statement
 *		  if ( expression ) statement else statement
 *		  assignment ;
 */

static void statement()
{
    if (lookahead == '{') {
	
	openScope();				

	match('{');
	declarations();
	statements();
	
	closeScope();
	
	match('}');

    } else if (lookahead == BREAK) {
	match(BREAK);
	match(';');

    } else if (lookahead == RETURN) {
	match(RETURN);
	expression();
	match(';');

    } else if (lookahead == WHILE) {
	match(WHILE);
	match('(');
	expression();
	match(')');
	statement();

    } else if (lookahead == FOR) {
	match(FOR);
	match('(');
	assignment();
	match(';');
	expression();
	match(';');
	assignment();
	match(')');
	statement();

    } else if (lookahead == IF) {
	match(IF);
	match('(');
	expression();
	match(')');
	statement();

	if (lookahead == ELSE) {
	    match(ELSE);
	    statement();
	}

    } else {
	assignment();
	match(';');
    }
}


/*
 * Function:	parameter
 *
 * Description:	Parse a parameter, which in Simple C is always a scalar
 *		variable with optional pointer declarators.
 *
 *		parameter:
 *		  specifier pointers identifier
 */

static Type parameter()
{
    int typespec = specifier();
    unsigned indirection = pointers();



	std::string nameID = lexbuf;
    match(ID);



	//i added this code so therefore it either works or it doesnt
	if(lookahead == '[') {

		match('[');
		unsigned long arrayLength = std::stoul(lexbuf, nullptr, 0);
		match(NUM);
		match(']');

		//cout << "(" << typespec << ", " << indirection << ", ARRAY)" << endl;
		
		Type varType(typespec, indirection, arrayLength); 
		declareVariable(nameID, varType);


		Type t1(typespec, indirection, arrayLength);
		return t1;

	} else {
		//cout << "(" << typespec << ", " << indirection << ", SCALAR)" << endl;

		Type varType(typespec, indirection); 
		declareVariable(nameID, varType);

		Type t1(typespec, indirection);
		return t1;
	}
	//end



}


/*
 * Function:	parameters
 *
 * Description:	Parse the parameters of a function, but not the opening or
 *		closing parentheses.
 *
 *		parameters:
 *		  void
 *		  parameter-list
 *		  parameter-list , ...
 */

static Parameters* parameters()
{
    Parameters* params = new Parameters(); //new used to allocate params on heap instead of stack
	
	if (lookahead == VOID)
		match(VOID);

    else {

		Type parameterType = parameter();
		params->types.push_back(parameterType);

		while (lookahead == ',') {
			match(',');

			if (lookahead == ELLIPSIS) {
				match(ELLIPSIS);
				params->variadic = true;
				return params;
			}

			Type parameterType = parameter();
			params->types.push_back(parameterType);
		}
    }
	
	params->variadic = false;
	return params;
}


/*
 * Function:	globalDeclarator
 *
 * Description:	Parse a declarator, which in Simple C is either a scalar
 *		variable, an array, or a function, with optional pointer
 *		declarators.
 *
 *		global-declarator:
 *		  pointers identifier
 *		  pointers identifier [ num ]
 *		  pointers identifier ( parameters )
 */

static void globalDeclarator(int typespec) 
{
	//typespec holds specifier int/char/long
	//params holds parameters if this is a function declaration
						 
    unsigned int indirection = pointers();

	std::string nameID = lexbuf;
    match(ID);

    if (lookahead == '(') {

	openScope();

	Parameters* params;

	match('(');
	params = parameters();
	match(')');

	Type declareType(typespec, indirection, params); 		//no function pointers right?

	declareFunction(nameID, declareType);
	closeScope();




    } else if (lookahead == '[') {

		match('[');
		unsigned long arrayLength = std::stoul(lexbuf, nullptr, 0);
		match(NUM);
		match(']');


		Type varType(typespec, indirection, arrayLength); 
		declareVariable(nameID, varType);

		//cout << "(" << typespec << ", " << indirection << ", ARRAY)" << endl;

    } else {

		Type varType(typespec, indirection); 
		declareVariable(nameID, varType);

//		cout << "(" << typespec << ", " << indirection << ", SCALAR)" << endl;

	}
}


/*
 * Function:	functionOrGlobal
 *
 * Description:	Parse a function definition or global declaration.
 *
 * 		function-or-global:
 * 		  specifier function-declarator { declarations statements }
 * 		  specifier global-declarator , global-declarator-list ;
 * 		  specifier global-declarator ;
 *
 * 		global-declarator-list:
 * 		  global-declarator
 * 		  global-declarator , global-declarator-list
 */

static void functionOrGlobal()
{
	
    int typespec = specifier();
    unsigned indirection = pointers();

	std::string nameID = lexbuf;
    match(ID);

    if (lookahead == '(') {

		openScope();

		Parameters* params;

		match('(');
		params = parameters();		
		match(')');

		Type declareType(typespec, indirection, params);
		

		if (lookahead == '{') {
			defineFunction(nameID, declareType);

			match('{');

			declarations();
			statements();

			closeScope();

			match('}');
			return;

		} else {
			
			declareFunction(nameID, declareType);
			closeScope();		
		}

    } else if (lookahead == '[') { 

		match('[');
		unsigned long arrayLength = std::stoul(lexbuf, nullptr, 0);
		match(NUM);
		match(']');
		

		Type varType(typespec, indirection, arrayLength); 
		declareVariable(nameID, varType);

		//cout << "(" << typespec << ", " << indirection << ", ARRAY)" << endl;

    } else {


		Type varType(typespec, indirection); 
		declareVariable(nameID, varType);

		//cout << "(" << typespec << ", " << indirection << ", SCALAR)" << endl;

	}


    while (lookahead == ',') {
		match(',');
		globalDeclarator(typespec);
    }

    match(';');

}


/*
 * Function:	main
 *
 * Description:	Analyze the standard input stream.
 *
 *		translation-unit:
 *		  empty
 *		  function-or-global translation-unit
 */

int main()
{

	//commented out for testing scope pls uncomment later
	
	openScope();	//open global scope
    lookahead = yylex();
    lexbuf = yytext;

    while (lookahead != DONE)
	functionOrGlobal();

	closeScope();	//close global scope

    cout << yylineno - 1 << endl;
    exit(EXIT_SUCCESS);
	
}
