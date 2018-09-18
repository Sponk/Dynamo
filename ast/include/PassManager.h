#ifndef DYNAMO_PASSMANAGER_H
#define DYNAMO_PASSMANAGER_H

#include "Node.h"
#include "Module.h"
#include "Assignment.h"
#include "Variable.h"
#include "Binop.h"
#include "Unop.h"
#include "FunctionDecl.h"
#include "Label.h"
#include "FunctionCall.h"
#include "While.h"
#include "For.h"
#include "If.h"
#include "Table.h"

namespace dynamo 
{

class PassManager
{
public:
	template<typename T>
	void run(T& pass, const Module& module)
	{
		for(auto& k : module.getNodes())
			run(pass, k);
	}
	
	template<typename T>
	void run(T& pass, NodeRef node)
	{
		if(!node)
			return;
		
		pass.handleNode(node);
		switch(node->getType())
		{
			case dynamo::BLOCK:
			{
				Block* b = reinterpret_cast<Block*>(node.get());
				for(auto n : b->getChildren())
					run(pass, n);
			}
			break;
			case dynamo::ASSIGNMENT:
			{
				Assignment* a = reinterpret_cast<Assignment*>(node.get());
				run(pass, a->getLHS());
				run(pass, a->getRHS());
			}
			break;
			
			case dynamo::BINOP:
			{
				Binop* b = reinterpret_cast<Binop*>(node.get());
				run(pass, b->getLHS());
				run(pass, b->getRHS());
			}
			break;
			
			case dynamo::UNOP:
			{
				Unop* u = reinterpret_cast<Unop*>(node.get());
				run(pass, u->getOperand());
			}
			break;
			
			case dynamo::FUNCTION_DECL:
			{
				FunctionDecl* f = reinterpret_cast<FunctionDecl*>(node.get());
				run(pass, f->getBody());
			}
			break;
			case dynamo::RETURN:
			{
				run(pass, node->getAccessor());
			}
			break;
			case dynamo::FUNCTION_CALL:
			{
				FunctionCall* f = reinterpret_cast<FunctionCall*>(node.get());
				for(auto& param : f->getParameters())
					run(pass, param);
			}
			break;
			case dynamo::WHILE:
			{
				While* w = reinterpret_cast<While*>(node.get());
				run(pass, w->getCheck());
				run(pass, w->getBody());
			}
			break;
			case dynamo::FOR:
			{
				For* f = reinterpret_cast<For*>(node.get());
				run(pass, f->getInit());
				run(pass, f->getCheck());
				run(pass, f->getIncrement());
				run(pass, f->getBody());
			}
			break;
			case dynamo::IF:
			{
				If* ifNode = reinterpret_cast<If*>(node.get());
				run(pass, ifNode->getCheck());
				run(pass, ifNode->getBody());
				run(pass, ifNode->getElse());
			}
			break;
			
			case dynamo::TABLE:
			{
				Table* table = reinterpret_cast<Table*>(node.get());
				for(auto& n : table->getEntries())
				{
					run(pass, n.first);
					run(pass, n.second);
				}
			}
			break;
			
			default: break;
		}
	}
};

}

#endif // DYNAMO_PASSMANAGER_H
