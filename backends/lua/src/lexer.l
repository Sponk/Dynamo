%option noyywrap
%option yylineno bison-locations

%{

#include <iostream>
#include <vector>

#include <Module.h>
#include <Unop.h>
#include <Binop.h>
#include <While.h>
#include <Assignment.h>
#include <For.h>
#include <FunctionCall.h>
#include <FunctionDecl.h>
#include <If.h>
#include <Variable.h>
#include <While.h>
#include <Table.h>

#include "parser.hh"

int yycolumn = -1;

#define YY_NO_UNPUT
#define YYERROR_VERBOSE

#define YY_USER_ACTION         \
	yylloc->first_column = yycolumn; \
	yycolumn += yyleng;            \
	yylloc->last_line = yylineno; \
	yylloc->last_column = yycolumn; 


%}

%%

"and" { return AND; }
"break" { return BREAK; }
"do" { return DO; }
"else" { return ELSE; }
"elseif" { return ELSEIF; }
"end" { return END; }
"false" { return FALSE; }
"for" { return FOR; }
"function" { return FUNCTION; }
"goto" { return GOTO; }
"if" { return IF; }
"in" { return IN; }
"local" { return LOCAL; }
"nil" { return NIL; }
"not" { return NOT; }
"or" { return OR; }
"repeat" { return REPEAT; }
"return" { return RETURN; }
"then" { return THEN; }
"true" { return TRUE; }
"until" { return UNTIL; }
"while" { return WHILE; }

"==" { return EQUAL; }
"~=" { return NOT_EQUAL; }
"<=" { return SMALLER_EQUAL; }
">=" { return GREATER_EQUAL; }
">>" { return SHIFT_RIGHT; }
"<<" { return SHIFT_LEFT; }
".." { return CONCAT; }

0x[0-9A-Fa-f]+|(-?)[0-9]+ { yylval->number = std::stoll(yytext); return NUMBER; }
(-?)[0-9]+\.[0-9+] { yylval->number = std::stod(yytext); return NUMBER; }
\"(([^\"]|\\\")*[^\\])?\" { yylval->string = strdup(yytext); return STRING; }

[a-zA-Z_][a-zA-Z0-9_]* { yylval->identifier = strdup(yytext); return IDENTIFIER; }
[+\-*/%^#&~|<>(){}:\.,;=\]\[] { return *yytext; }

--.* ;
[ \t] ;
[\n] { yycolumn = 1; }

%%
