/*
 * File:	parser.cpp
 *
 * Description:	This file is just a placeholder for the real parser.  It is
 *		here just so that the main function is separate from the
 *		lexer and can be built upon in a separate file.
 */

# include <cerrno>
# include <cstdlib>
# include <iostream>
# include "lexer.h" 
# include "tokens.h"

using namespace std;

static int lookahead, nexttoken;
string lexbuf, nextbuf;

void expression();
void expressionlist();

void fog();

void globaldeclarator();
void globaldeclaratorlist();

void andexpression();
void equality();
void relational();
void E();
void T();
void prefix();
void postfix();
void primary();

void parameter();
void parameters();
void parameterlist();

void specifier();
void pointers();

void statement();
void statements();

void declaration();
void declarator();
void declaratorlist();
void declarations();

void assignment();



void error(){                                       //call in match, specifier, and primary
    //cerr << "error function called" << endl;
    report("syntax error: %s", lexbuf);
    exit(1);
}

int peek(){
    if(!nexttoken){
        nexttoken = yylex();
        nextbuf = yytext;
    }

    return nexttoken;
}


int isSpecifier(int token){
    if(token == CHAR || token == INT || token == LONG){
        return true;
    } else {
        return false;
    }
    
}

void match(int t) {
    if(lookahead != t){
        error();
    } 

    if(nexttoken) { 
        lookahead = nexttoken;
        lexbuf = nextbuf;
        nexttoken = 0;


    } else {
        lookahead = yylex();
        lexbuf = yytext;
    }
}




int main()
{
    lookahead = yylex();

    while(lookahead != DONE) {
        fog();
        //match(';');
    }

    cout << yylineno -1 << endl;
    return 0;
}



void fog(){

    specifier();
    pointers();
    match(ID);

    if(lookahead == '('){

        match('(');
        parameters();
        match(')');
    
        if(lookahead == '{'){
            match('{');
            declarations();
            statements();
            match('}');
            return;
        }

    } else if (lookahead == '[') {
        match('[');
        match(NUM);
        match(']');
    } 

    while(lookahead==','){
        match(',');
        globaldeclarator();     
    }

    match(';');
}



void globaldeclarator(){
    pointers();
    match(ID);

    if(lookahead == '['){
        match('[');
        match(NUM);
        match(']');
    } else if (lookahead == '('){
        
        match('(');
        parameters();
        match(')');

    } 
}




void expression(){
    andexpression();

    while(1){
        
        if(lookahead == OR){

            match(OR);
            andexpression();
            cout << "or" << endl;

        } else {
            break;
        }
    }
}


void andexpression(){
    equality();

    while(1){
        
        if(lookahead == AND){

            match(AND);
            equality();
            cout << "and" << endl;

        } else {
            break;
        }
    }
}



void equality(){
    relational();

    while(1){
         
        if(lookahead == EQL){

            match(EQL);
            relational();
            cout << "eql" << endl;
            
            

        } else if(lookahead == NEQ){

            match(NEQ);
            relational();
            cout << "neq" << endl;

        } else {
            break;
        }
    }

}

void relational(){
    E();

    while(1){

        if(lookahead == '<'){

            match('<');
            E();
            cout << "ltn" << endl;
            
        } else if(lookahead == '>'){
            
            match('>');
            E();
            cout << "gtn" << endl;

        } else if (lookahead == LEQ) {

            match(LEQ);
            E();
            cout << "leq" << endl;

        } else if (lookahead == GEQ) {
            
            match(GEQ);
            E();
            cout << "geq" << endl;

        } else {
            break;
        }
    }
}



void E(){
    T();
    
    while(1){

        if(lookahead == '+'){
            
            match('+');
            T();
            cout << "add" << endl;

        } else if(lookahead == '-'){
            
            match('-');
            T();
            cout << "sub" << endl;

        } else {
            break;
        }
    }
}




void T(){
    prefix();

    while(1){

        if(lookahead == '*'){
            
            match('*');
            prefix();
            cout << "mul" << endl;

        } else if(lookahead == '/'){
            
            match('/');
            prefix();
            cout << "div" << endl;

        } else if (lookahead == '%'){

            match('%');
            prefix();
            cout << "rem" << endl;

        } else {
            break;
        }

    }


}

void prefix(){

   if(lookahead == '!'){
            
        match('!');
        prefix();
        cout << "not" << endl;

    } else if(lookahead == '-'){
        
        match('-');
        prefix();
        cout << "neg" << endl;

    }else if (lookahead == '&'){

        match('&');
        prefix();
        cout << "addr" << endl;

    } else if (lookahead == '*'){

        match('*');
        prefix();
        cout << "deref" << endl;



    //cast & sizeof

    // (int *) a;
    } else if (lookahead == '(' && isSpecifier(peek())){
        match('(');
        specifier();
        pointers();
        match(')');

        prefix();
        
        cout << "cast" << endl;

    
    // sizeof(int **)
    // sizeof a
    // sizeof(a)

    } else if (lookahead == SIZEOF) {
        
        match(SIZEOF);

        if(lookahead == '(' && isSpecifier(peek())){
            match('(');
            specifier();
            pointers();
            match(')');
        } else {
            prefix();
        }

        cout << "sizeof" << endl;

        
    } else {
        postfix();
    }       
}
  


void postfix(){
    primary();

    while (1){
        if (lookahead == '['){

            match('[');
            expression();
            match(']');

            cout << "index" << endl;
        
        } else {
            break;
        }
    }
}


void primary(){

    if (lookahead == '(') {

        match('(');
        expression();
        match(')');

    } else if (lookahead == NUM){
        match(NUM);

    } else if (lookahead == CHARACTER) {
        match(CHARACTER);

    } else if (lookahead == STRING){
        match(STRING);

    } else {

        match(ID);

        if(lookahead == '('){      
            match('(');

            if(lookahead == ')'){   //empty paren
                match(')');

            } else {                //expression list in paren 
                
                expressionlist(); 
                match(')');
            }
        }
    }
}


void expressionlist(){

    expression();

    while(lookahead == ','){ 
        match(',');
        expression();
    }

}



void specifier(){                 
    if(lookahead == CHAR){     
        match(CHAR);
    } else if (lookahead == INT){
        match(INT);
    } else if (lookahead == LONG){
        match(LONG);
    } else {

        cerr << "specifier error" << endl;
    }
}



void pointers(){                      

    while(lookahead == '*'){
        match('*');
    }
}





void statement(){
    
    if(lookahead == '{'){
            
        match('{');
        declarations();
        statements();
        match('}');

    } else if (lookahead == BREAK){
            
        match(BREAK);
        match(';');
        return;

    } else if (lookahead == RETURN){
            
        match(RETURN);
        expression();
        match(';');
        return;

    } else if (lookahead == WHILE){
        
        match(WHILE);
        match('(');
        expression();
        match(')');
        statement();
        return;
    
    } else if (lookahead == FOR){
        
        match(FOR);
        match('(');
        assignment();
        match(';');
        expression();
        match(';');
        assignment();
        match(')');
        statement();
        return;

    } else if (lookahead == IF){
        
        match(IF);
        match('(');
        expression();
        match(')');
        statement();
        

        if(lookahead == ELSE){
            match(ELSE);
            statement();
        }
        return;
    
    }else{

        assignment();
        match(';');
        return;
    }
}


void assignment(){
    expression();
    if(lookahead == '='){
        match('=');
        expression();
    }
}





void declaration(){
    specifier();
    declaratorlist();
    match(';');
}


void declaratorlist(){
    declarator();
    if(lookahead == ','){
        match(',');
        declaratorlist();
    }
}



void declarator(){
    
    pointers();
    match(ID);
    if (lookahead == '['){
        
        match('[');
        match(NUM);
        match(']');
    }
}





void statements(){

    if(lookahead != '}'){
        statement();
        statements();
    }
}




void declarations(){

        if(isSpecifier(lookahead)){
            declaration();
            declarations();
        }
}




void parameter(){
    specifier();
    pointers();
    match(ID);
}



void parameters(){                          //problem here

    //if(lookahead == ')') return; //void?
    if(lookahead == VOID){
        match(VOID);
    } else {
        parameterlist();
    }

    
}

void parameterlist(){
    parameter();

    if(lookahead == ','){
    
        match(',');
        if(lookahead == ELLIPSIS) {
            match(ELLIPSIS);
        } else {
            parameterlist();
        }
    }
}





