
%locations
%define api.pure full

%{

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <cassert>

#include <memory>

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
#include <Value.h>
#include <Return.h>
#include <Table.h>
#include <Goto.h>
#include <Label.h>
#include <Break.h>

using namespace std;

#include "parser.hh"

int yylex(YYSTYPE*, YYLTYPE*);
int yyparse();

extern FILE *yyin;
extern char *yytext;
 
void yyerror(YYLTYPE*, char const* msg);

#define YY_NO_UNPUT
#define YYERROR_VERBOSE

using namespace dynamo;
std::shared_ptr<Module> root;

%}

%union{
	char* identifier;
	char* string;
	double number;
	
	dynamo::Node* node;
	dynamo::Unop* unop;
	dynamo::Binop* binop;
	dynamo::Assignment* assignment;
	dynamo::For* forloop;
	dynamo::While* whileloop;
	dynamo::If* ifstmt;
	dynamo::Variable* variable;
	dynamo::FunctionDecl* function;
	
	std::vector<dynamo::NodeRef>* nodelist;
	std::vector<std::string>* identifierlist;
	
	std::pair<dynamo::NodeRef, dynamo::NodeRef>* nodepair;
	std::vector<std::pair<dynamo::NodeRef, dynamo::NodeRef>>* nodepairlist;
}

%start parser

%token <identifier> IDENTIFIER
%token <string> STRING
%token <number> NUMBER

%token AND
%token BREAK
%token DO
%token ELSE
%token ELSEIF
%token END
%token FALSE
%token FOR
%token FUNCTION
%token GOTO
%token IF
%token IN
%token LOCAL
%token NIL
%token NOT
%token OR
%token REPEAT
%token RETURN
%token THEN
%token TRUE
%token UNTIL
%token WHILE

%token EQUAL
%token NOT_EQUAL
%token SMALLER_EQUAL
%token GREATER_EQUAL
%token SHIFT_RIGHT
%token SHIFT_LEFT
%token CONCAT

%type <node> stat
%type <nodelist> statlist
%type <nodelist> explist
%type <identifierlist> identifierlist
%type <node> exp
%type <node> prefixexp
// %type <node> var
%type <node> name
%type <node> funcname
%type <function> functiondef
%type <function> funcbody
%type <identifierlist> parlist
%type <node> retstat
%type <nodelist> args
%type <node> functioncall
%type <nodelist> elselist
%type <nodepair> field;
%type <nodepairlist> fieldlist;
%type <node> tableconstructor;
%type <node> label;
%type <node> var;
%type <nodelist> varlist

%left AND OR
%left '<' '>' '=' EQUAL NOT_EQUAL SMALLER_EQUAL GREATER_EQUAL
%right CONCAT
%left '+' '-' SHIFT_RIGHT SHIFT_LEFT
%left '*' '/'

%left NOT '|' '&' '%'
%right '^'

%left '#' '~'
%precedence '.' ':'

%%

parser: statlist
		{
			for(auto k : *$1)
			{
				assert(k != nullptr);
				root->addNode(NodeRef(k));
			}
			
			delete $1;
		}
	;
	
statlist: { $$ = new std::vector<NodeRef>(); }
	| stat 
		{ 
			$$ = new std::vector<NodeRef>();
			
			if($1 != nullptr)
				$$->push_back(NodeRef($1));
		}

	| statlist stat
		{
			$$ = $1;
			if($2 != nullptr)
				$$->push_back(NodeRef($2));
		}

stat: ';' { $$ = nullptr; }
	| varlist '=' explist 
		{ 
			assert($1->size() == $3->size());
			auto block = new Block();
			for(size_t i = 0; i < $1->size(); i++)
			{
				block->addChild(new Assignment(
							(*$1)[i], // lhs
							(*$3)[i], // rhs
							false)); // Local
			}
			
			$$ = block;
		}
	| functioncall { $$ = $1; }
	| label { $$ = $1; }
	| BREAK { $$ = new Break(); }
	| GOTO IDENTIFIER { $$ = new Goto($2); }
	| DO statlist END 
		{ 
			auto block = new Block();
			block->getChildren() = std::move(*$2);
			$$ = block;
		}
	
	| WHILE exp DO statlist END 
		{
			auto loop = new While();
			
			auto block = new Block();
			block->getChildren() = std::move(*$4);

			loop->setBody(NodeRef(block));
			loop->setCheck(NodeRef($2));
			loop->setDoWhile(false);
			$$ = loop; 
		}
	
	| REPEAT statlist UNTIL exp
		{
			auto loop = new While();
			auto block = new Block();
			block->getChildren() = std::move(*$2);

			loop->setBody(NodeRef(block));
			loop->setCheck(NodeRef($4));
			loop->setDoWhile(true);
			$$ = loop; 
		}
	
	| FOR IDENTIFIER '=' exp ',' exp DO statlist END 
		{
			auto loop = new For();
			auto block = new Block();
			block->getChildren() = std::move(*$8);
			
			loop->setBody(NodeRef(block));
			loop->setCheck(NodeRef($6));
			loop->setIncrement(nullptr);
			loop->setInit(std::make_shared<Assignment>(std::make_shared<Variable>($2), NodeRef($4)));
			
			$$ = loop;
		}
		
	| FOR IDENTIFIER '=' exp ',' exp ',' exp DO statlist END
		{
			auto loop = new For();
			auto block = new Block();
			block->getChildren() = std::move(*$10);
			
			loop->setBody(NodeRef(block));
			loop->setCheck(NodeRef($6));
			loop->setIncrement(NodeRef($8));
			loop->setInit(std::make_shared<Assignment>(std::make_shared<Variable>($2), NodeRef($4)));
			
			$$ = loop;
		}
	
	| FOR identifierlist IN explist DO statlist END { $$ = new Value(VALUE_NIL); }
	
	| FUNCTION funcname funcbody 
		{
			$3->setName(NodeRef($2));
			$$ = $3;
		}
	
	| LOCAL FUNCTION IDENTIFIER funcbody 
		{ 
			$4->setName(std::make_shared<Variable>($3));
			$4->setLocal(true);
			$$ = $4;
		}
	| LOCAL identifierlist 
		{ 
			auto block = new Block();
			for(size_t i = 0; i < $2->size(); i++)
			{
				block->addChild(new Assignment(
							std::make_shared<Variable>(std::move((*$2)[i])), // lhs
							std::make_shared<Value>(VALUE_NIL), // rhs
							true)); // Local
			}
			
			$$ = block;
		}
	| LOCAL varlist '=' explist 
		{
			assert($2->size() == $4->size());
			auto block = new Block();
			for(size_t i = 0; i < $2->size(); i++)
			{
				block->addChild(new Assignment(
							(*$2)[i], // lhs
							(*$4)[i], // rhs
							true)); // Local
			}
			
			$$ = block;
		}
	
	| IF exp THEN statlist END
		{ 
			auto i = new If();
			auto block = new Block();
			block->getChildren() = std::move(*$4);
			
			i->setCheck(NodeRef($2));
			i->setBody(NodeRef(block));
			
			$$ = i; 
		}
		
	| IF exp THEN statlist ELSE statlist END
		{ 
			auto i = new If();
			auto ifBlock = new Block();
			ifBlock->getChildren() = std::move(*$4);
			
			i->setCheck(NodeRef($2));
			i->setBody(NodeRef(ifBlock));
			
			auto e = new If();
			auto elseBlock = new Block();
			elseBlock->getChildren() = std::move(*$6);
			e->setBody(NodeRef(elseBlock));
			
			i->setElse(NodeRef(e));
			
			$$ = i; 
		}
		
	| IF exp THEN statlist elselist END
		{ 
			auto i = new If();
			auto block = new Block();
			block->getChildren() = std::move(*$4);
			
			i->setCheck(NodeRef($2));
			i->setBody(NodeRef(block));
			
			for(size_t i = 0; i < $5->size() - 1; i++)
			{
				std::reinterpret_pointer_cast<If>((*$5)[i])->setElse((*$5)[i + 1]);
			}
			
			i->setElse((*$5)[0]);
			$$ = i; 
		}
	
	| retstat  { $$ = $1; }
	;

elselist:
	ELSEIF exp THEN statlist
		{
			auto i = new If();
			auto block = new Block();
			block->getChildren() = std::move(*$4);
			
			i->setCheck(NodeRef($2));
			i->setBody(NodeRef(block));
			
			$$ = new std::vector<NodeRef>({ NodeRef(i) });
		}
	| elselist ELSEIF exp THEN statlist
		{
			auto i = new If();
			auto block = new Block();
			block->getChildren() = std::move(*$5);
			
			i->setCheck(NodeRef($3));
			i->setBody(NodeRef(block));
			
			$1->push_back(NodeRef(i));
			$$ = $1;
		}
	| elselist ELSE statlist
		{
			auto i = new If();
			auto block = new Block();
			block->getChildren() = std::move(*$3);
				
			i->setBody(NodeRef(block));
				
			$1->push_back(NodeRef(i));
			$$ = $1;
		}
	;

retstat:  RETURN ';' { $$ = new Return(); }
	| RETURN { $$ = new Return(); }
	| RETURN explist ';' 
		{ 
			$$ = new Return(); 
			
			auto block = new Block();
			block->getChildren() = std::move(*$2);
			
			$$->setAccessor(NodeRef(block));
		}
	| RETURN explist
		{ 
			$$ = new Return(); 
			
			auto block = new Block();
			block->getChildren() = std::move(*$2);
			
			$$->setAccessor(NodeRef(block));
		}
	;
	
label: ':'':' IDENTIFIER ':'':' { $$ = new Label($3); delete $3; };

name: IDENTIFIER { $$ = new Variable($1); }
	| name '.' IDENTIFIER 
		{ 
			$$ = $1;

			auto iter = $$;
			while(iter->getAccessor()) iter = iter->getAccessor().get();
			iter->setAccessor(std::make_shared<Value>(VALUE_STRING, std::string("\"") + $3 + "\""));
			
			delete $3;
		}
		;
	
funcname:
	name { $$ = $1; }
	| name ':' IDENTIFIER
		{
			$$ = $1;
			
			auto iter = $$;
			while(iter->getAccessor()) iter = iter->getAccessor().get();
			iter->setAccessor(std::make_shared<Value>(VALUE_STRING, std::string("\"") + $3 + "\""));
			iter->setStaticAccess(false);

			delete $3;
		}
	;
	
varlist: var { $$ = new std::vector<NodeRef>({ NodeRef($1) }); }
	| varlist ',' var 
		{ 
			$$ = $1; 
			$$->push_back(NodeRef($3)); 
		}
	;
	
var:
	prefixexp { $$ = $1; }
	| prefixexp '[' exp ']' 
		{
			$$ = $1;
			
			auto iter = $$;
			while(iter->getAccessor()) iter = iter->getAccessor().get();
			iter->setAccessor(NodeRef($3));
		}
	| prefixexp '.' name
		{
			$$ = $1;
						
			auto iter = $$;
			while(iter->getAccessor()) iter = iter->getAccessor().get();
			iter->setAccessor($3);
		}
	;
	
identifierlist: IDENTIFIER
			{
				$$ = new std::vector<std::string>();
				$$->push_back($1);
				delete $1;
			}
			
		| identifierlist ',' IDENTIFIER
			{
				$$ = $1;
				$$->push_back($3);
				delete $3;
			}
		;
		
explist: exp
		{
			$$ = new std::vector<NodeRef>();
			$$->push_back(NodeRef($1));
		}
		
	| explist ',' exp
		{
			$$ = $1;
			$$->push_back(NodeRef($3));
		}
	;
	
exp: 
	NIL { $$ = new Value(VALUE_NIL); }
	| FALSE { $$ = new Value(VALUE_BOOLEAN, "false"); }
	| TRUE { $$ = new Value(VALUE_BOOLEAN, "true"); }
	| NUMBER { $$ = new Value(VALUE_NUMBER, std::to_string($1)); }
	| STRING { $$ = new Value(VALUE_STRING, $1); }
	| '.''.''.'  { $$ = new Value(VALUE_NIL); }
	| functiondef { $$ = $1; }
	| var  { $$ = $1; }
	| tableconstructor  { $$ = $1; }
	
	| exp '+' exp { auto n = new Binop("+"); n->setRHS(NodeRef($3)); n->setLHS(NodeRef($1)); $$ = n; }
	| exp '-' exp { auto n = new Binop("-"); n->setRHS(NodeRef($3)); n->setLHS(NodeRef($1)); $$ = n; }
	| exp '*' exp { auto n = new Binop("*"); n->setRHS(NodeRef($3)); n->setLHS(NodeRef($1)); $$ = n; }
	| exp '/' exp { auto n = new Binop("/"); n->setRHS(NodeRef($3)); n->setLHS(NodeRef($1)); $$ = n; }
	| exp '/''/' exp { auto n = new Binop("//"); n->setRHS(NodeRef($4)); n->setLHS(NodeRef($1)); $$ = n; } 
	| exp '^' exp { auto n = new Binop("^"); n->setRHS(NodeRef($3)); n->setLHS(NodeRef($1)); $$ = n; }
	| exp '%' exp { auto n = new Binop("%"); n->setRHS(NodeRef($3)); n->setLHS(NodeRef($1)); $$ = n; }
	| exp '&' exp { auto n = new Binop("&"); n->setRHS(NodeRef($3)); n->setLHS(NodeRef($1)); $$ = n; }
	| exp '~' exp { auto n = new Binop("~"); n->setRHS(NodeRef($3)); n->setLHS(NodeRef($1)); $$ = n; }
	| exp '|' exp { auto n = new Binop("|"); n->setRHS(NodeRef($3)); n->setLHS(NodeRef($1)); $$ = n; }
	| exp SHIFT_RIGHT exp { auto n = new Binop(">>"); n->setRHS(NodeRef($3)); n->setLHS(NodeRef($1)); $$ = n; }
	| exp SHIFT_LEFT exp { auto n = new Binop("<<"); n->setRHS(NodeRef($3)); n->setLHS(NodeRef($1)); $$ = n; }
	| exp CONCAT exp { auto n = new Binop(".."); n->setRHS(NodeRef($3)); n->setLHS(NodeRef($1)); $$ = n; }
	| exp '<' exp { auto n = new Binop("<"); n->setRHS(NodeRef($3)); n->setLHS(NodeRef($1)); $$ = n; }
	| exp SMALLER_EQUAL exp { auto n = new Binop("<="); n->setRHS(NodeRef($3)); n->setLHS(NodeRef($1)); $$ = n; }
	| exp '>' exp { auto n = new Binop(">"); n->setRHS(NodeRef($3)); n->setLHS(NodeRef($1)); $$ = n; }
	| exp GREATER_EQUAL exp { auto n = new Binop(">="); n->setRHS(NodeRef($3)); n->setLHS(NodeRef($1)); $$ = n; }
	| exp EQUAL exp { auto n = new Binop("=="); n->setRHS(NodeRef($3)); n->setLHS(NodeRef($1)); $$ = n; }
	| exp NOT_EQUAL exp { auto n = new Binop("~="); n->setRHS(NodeRef($3)); n->setLHS(NodeRef($1)); $$ = n; }
	| exp AND exp { auto n = new Binop("&&"); n->setRHS(NodeRef($3)); n->setLHS(NodeRef($1)); $$ = n; }
	| exp OR exp { auto n = new Binop("||"); n->setRHS(NodeRef($3)); n->setLHS(NodeRef($1)); $$ = n; }
	
	| '-' exp { auto n = new Unop("-"); n->setOperand(NodeRef($2)); $$ = n; } 
	| NOT exp { auto n = new Unop("!"); n->setOperand(NodeRef($2)); $$ = n; }
	| '#' exp { auto n = new Unop("#"); n->setOperand(NodeRef($2)); $$ = n; }
	| '~' exp { auto n = new Unop("~"); n->setOperand(NodeRef($2)); $$ = n; };
	;

prefixexp: name { $$ = $1; }
	| functioncall { $$ = $1; }
	| '(' exp ')' { $$ = $2; }
	;

functioncall: prefixexp args 
		{
			auto call = new FunctionCall();
			call->getParameters() = std::move(*$2);
			call->setAccessor(NodeRef($1));
			$$ = call;
		}
	| prefixexp ':' IDENTIFIER args
		{
			auto call = new FunctionCall();
			call->getParameters() = std::move(*$4);
			call->setAccessor(NodeRef($1));
			
			auto iter = $$;
			while(iter->getAccessor()) iter = iter->getAccessor().get();
			iter->setAccessor(std::make_shared<Value>(VALUE_STRING, std::string("\"") + $3 + "\""));
			iter->getAccessor()->setStaticAccess(false);
			
			$$ = call;
		}

args: '(' ')' { $$ = new std::vector<NodeRef>(); }
	| tableconstructor { $$ = new std::vector<NodeRef>({ NodeRef($1) }); }
	| STRING { $$ = new std::vector<NodeRef>({ std::make_shared<Value>(VALUE_STRING, $1) }); }
	| '(' explist ')' { $$ = $2; }
	;
	
functiondef: FUNCTION funcbody { $$ = $2; };

funcbody: '(' ')' statlist END
		{
			auto func = new FunctionDecl();
			
			auto block = new Block();
			block->getChildren() = std::move(*$3);
			func->setBody(NodeRef(block));
			
			$$ = func;
		}
	| '(' parlist ')' statlist END
		{
		
			auto func = new FunctionDecl();
			func->setArguments(std::move(*$2));
			
			auto block = new Block();
			block->getChildren() = std::move(*$4);
			func->setBody(NodeRef(block));
			
			delete $4;
			delete $2;
			
			$$ = func;
		}
	;
	
parlist: identifierlist { $$ = $1; }
	| "..." { $$ = new std::vector<std::string>({"..."}); }
	| identifierlist ',' "..." 
		{
			$$ = $1;
			$$->push_back("...");
		};
	
tableconstructor: '{' '}' { $$ = new Table(); }
	| '{' fieldlist '}'
		{
			auto table = new Table();
			table->getEntries() = std::move(*$2);
			$$ = table;
			
			delete $2;
		}
	;
	
fieldlist: field { $$ = new std::vector<std::pair<NodeRef, NodeRef>>({ *$1 }); delete $1; }
	| fieldlist fieldsep field 
		{
			$$ = $1; 
			$1->push_back(*$3);
			delete $3;
		}
	| fieldlist fieldsep { $$ = $1; }
	;
	
field: '[' exp ']' '=' exp 
		{  
			$$ = new std::pair<NodeRef, NodeRef>($2, $5);
		}
	| IDENTIFIER '=' exp
		{  
			$$ = new std::pair<NodeRef, NodeRef>(new Variable($1), $3);
		}
	| exp
		{  
			$$ = new std::pair<NodeRef, NodeRef>(nullptr, $1);
		}
	;
	
fieldsep: ',' | ';';

%%

#define LEXER_IMPLEMENTED

int parse(FILE *fp)
{
	yyin = fp;

	do {
#ifdef LEXER_IMPLEMENTED
		yyparse();
#else
		int x;
		std::cout << "Resulting tokens: ";
		while (x = yylex())
		{
			std::cout << "<" << yytext << "> ";
		}
		std::cout << std::endl;
#endif

	} while(!feof(yyin));
#ifndef LEXER_IMPLEMENTED
	std::exit(EXIT_SUCCESS);
#endif

	return 0;
}

static bool error = false;
void yyerror(YYLTYPE* loc, char const* msg)
{
	std::cerr << "Parser error: " << msg << " at " << loc->last_line << ":" << loc->first_column << std::endl;
	error = true;
	// std::exit(EXIT_FAILURE);
}

#include <PassManager.h>
#include "LuaForLoopConversion.hpp"
#include "LuaOperatorConversion.hpp"

std::shared_ptr<Module> parse(const std::string& file, int argc, char** argv)
{
	if(root)
		throw std::runtime_error("Parser is not reentrant!");
		
	root = std::make_shared<Module>();
	
	FILE* f = fopen(file.c_str(), "r");
	if(!f)
		throw std::runtime_error("Could not open file: " + file);
		
	error = false;
	parse(f);
	fclose(f);

	auto retval = std::move(root);
	
	PassManager pm;
	LuaForLoopConversion pass;
	LuaOperatorConversion operatorConv;
	
	pm.run(pass, *retval);
	pm.run(operatorConv, *retval);
	
	if(error)
		retval = nullptr;
		
	return retval;
}

