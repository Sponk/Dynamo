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
	void handleNode(dynamo::NodeRef node) override
	{
		if(node->getType() == dynamo::FOR)
		{
			auto loop = reinterpret_cast<dynamo::For*>(node.get());
			auto init = reinterpret_cast<dynamo::Assignment*>(loop->getInit().get());
			
			auto check = loop->getCheck();
			auto newCheck = std::make_shared<dynamo::Binop>("<=");
			
			newCheck->setLHS(init->getLHS());
			newCheck->setRHS(check);
			
			auto inc = loop->getIncrement();
			auto newInc = std::make_shared<dynamo::Binop>("+=");
			
			newInc->setLHS(init->getLHS());
			newInc->setRHS(inc != nullptr ? inc : std::make_shared<Value>(VALUE_NUMBER, "1.0"));
			
			// All loop counter are local to the scope in Lua
			init->setLocal(true);
			
			loop->setCheck(newCheck);
			loop->setIncrement(newInc);
		}
	}
};
