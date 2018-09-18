#pragma once
#include <Pass.h>
#include <Node.h>
#include <For.h>
#include <Assignment.h>
#include <Binop.h>
#include <vector>

class LuaOperatorConversion: public dynamo::Pass
{
public:
	void handleNode(dynamo::Node* node) override
	{
		if(node->getType() == dynamo::BINOP)
		{
			auto binop = reinterpret_cast<Binop*>(node);
			if(binop->getOperator() == "~=")
				binop->setOperator("!=");
			else if(binop->getOperator() == "^")
				binop->setOperator("**");
			else if(binop->getOperator() == "..")
				binop->setOperator("+");
		}
		else if(node->getType() == dynamo::UNOP)
		{
			auto unop = reinterpret_cast<Unop*>(node);
			if(unop->getOperator() == "#")
				unop->setOperator("*");
		}
	}
};
