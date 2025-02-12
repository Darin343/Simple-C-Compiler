%{

/*
 * File:	lexer.l
 *
 * Description:	This file contains the flex description for the lexical
 *		analyzer for Simple C.
 */

#include <iostream>

using namespace std;

bool isComment();

%}

%option nounput noyywrap
%%


"auto"		                    { cout << "keyword " << yytext << endl; }
"break"                             { cout << "keyword " << yytext << endl; }
"case"                              { cout << "keyword " << yytext << endl; }
"char"                              { cout << "keyword " << yytext << endl; }
"const"                             { cout << "keyword " << yytext << endl; }
"continue"                          { cout << "keyword " << yytext << endl; }
"default"                           { cout << "keyword " << yytext << endl; }
"do"                                { cout << "keyword " << yytext << endl; }
"double"                            { cout << "keyword " << yytext << endl; }
"else"                              { cout << "keyword " << yytext << endl; }
"enum"                              { cout << "keyword " << yytext << endl; }
"extern"                            { cout << "keyword " << yytext << endl; }
"float"                             { cout << "keyword " << yytext << endl; }
"for"                               { cout << "keyword " << yytext << endl; }
"goto"                              { cout << "keyword " << yytext << endl; }
"if"                                { cout << "keyword " << yytext << endl; }
"int"                               { cout << "keyword " << yytext << endl; }
"long"                              { cout << "keyword " << yytext << endl; }
"register"                          { cout << "keyword " << yytext << endl; }
"return"                            { cout << "keyword " << yytext << endl; }
"short"                             { cout << "keyword " << yytext << endl; }
"signed"                            { cout << "keyword " << yytext << endl; }
"sizeof"                            { cout << "keyword " << yytext << endl; }
"static"                            { cout << "keyword " << yytext << endl; }
"struct"                            { cout << "keyword " << yytext << endl; }
"switch"                            { cout << "keyword " << yytext << endl; }
"typedef"                           { cout << "keyword " << yytext << endl; }
"union"                             { cout << "keyword " << yytext << endl; }
"unsigned"                          { cout << "keyword " << yytext << endl; }
"void"                              { cout << "keyword " << yytext << endl; }
"volatile"                          { cout << "keyword " << yytext << endl; }
"while"                             { cout << "keyword " << yytext << endl; }



"="                                 { cout << "operator " << yytext << endl; }
"||"                                  { cout << "operator " << yytext << endl; }
"&&"                                  { cout << "operator " << yytext << endl; }
"|"                                  { cout << "operator " << yytext << endl; }
"=="                                  { cout << "operator " << yytext << endl; }
"!="                                  { cout << "operator " << yytext << endl; }
"<"                                  { cout << "operator " << yytext << endl; }
">"                                  { cout << "operator " << yytext << endl; }
"<="                                  { cout << "operator " << yytext << endl; }
">="                                  { cout << "operator " << yytext << endl; }
"+"                                  { cout << "operator " << yytext << endl; }
"-"                                  { cout << "operator " << yytext << endl; }
"*"                                  { cout << "operator " << yytext << endl; }
"/"                                  { cout << "operator " << yytext << endl; }
"%"                                  { cout << "operator " << yytext << endl; }
"&"                                  { cout << "operator " << yytext << endl; }
"!"                                  { cout << "operator " << yytext << endl; }
"++"                                  { cout << "operator " << yytext << endl; }
"--"                                  { cout << "operator " << yytext << endl; }
"->"                                  { cout << "operator " << yytext << endl; }
"(" 				   { cout << "operator " << yytext << endl; }
")"                                  { cout << "operator " << yytext << endl; }
"[" 					{ cout << "operator " << yytext << endl; } 
"]"                                  { cout << "operator " << yytext << endl; }
"{" 					{ cout << "operator " << yytext << endl; }
"}"                                  { cout << "operator " << yytext << endl; }
";"                                  { cout << "operator " << yytext << endl; }
":"                                  { cout << "operator " << yytext << endl; }
"."                                  { cout << "operator " << yytext << endl; }
","                                  { cout << "operator " << yytext << endl; }
"..."                                  { cout << "operator " << yytext << endl; }




"/*" 				    { isComment(); }


[a-zA-Z_][a-zA-Z_0-9]*		    { cout << "identifier " << yytext << endl; }

[0-9]+[lL]?		            { cout << "number " << yytext << endl; }

\"([^\\\n\"]|(\\.))*\"		    { cout << "string " << yytext << endl; }

\'([^\\\n\']|(\\.))+\'		    { cout << "character " << yytext << endl; }


[ \f\n\r\t\v]+			        {/* ignored */}

.						        {/* ignored */}

%%

int main() {
	yylex();
	return 0;
}




bool isComment(){

        char p2 = yyinput();
        char p1 = ' ';

        while(true){
                if (p1 == '*' && p2 == '/') {
                        return true;
                } else {
                        p1 = p2;
                        p2 = yyinput();
                }
        
	}
}
