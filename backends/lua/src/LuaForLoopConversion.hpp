#pragma once
#include <Pass.h>
#include <Node.h>
#include <For.h>
#include <Assignment.h>
#include <Binop.h>
#include <vector>

class LuaForLoopConversion: public dynamo::Pass
{
public:
	void handleNode(dynamo::Node* node) override
	{
		if(node->getType() == dynamo::FOR)
		{
			auto loop = reinterpret_cast<dynamo::For*>(node);
			auto init = reinterpret_cast<dynamo::Assignment*>(loop->getInit());
			
			auto check = loop->getCheck();
			auto newCheck = new dynamo::Binop("!=");
			
			newCheck->setLHS(init->getLHS());
			newCheck->setRHS(check);
			
			auto inc = loop->getIncrement();
			auto newInc = new dynamo::Binop("+=");
			
			newInc->setLHS(init->getLHS());
			newInc->setRHS(inc != nullptr ? inc : new Value(VALUE_NUMBER, "1.0"));
			
			// All loop counter are local to the scope in Lua
			init->setLocal(true);
			
			loop->setCheck(newCheck);
			loop->setIncrement(newInc);
		}
	}
};
