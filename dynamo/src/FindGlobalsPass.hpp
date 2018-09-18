#pragma once
#include <Pass.h>
#include <Node.h>
#include <Assignment.h>
#include <vector>

class FindGlobalsPass: public dynamo::Pass
{
	std::vector<dynamo::Assignment*> m_globals;
public:
	void handleNode(dynamo::Node* node) override
	{
		if(node->getType() == dynamo::ASSIGNMENT)
		{
			auto assign = reinterpret_cast<dynamo::Assignment*>(node);
			if(!assign->isLocal())
				m_globals.push_back(assign);
		}
	}
	
	std::vector<dynamo::Assignment*>& getGlobals() { return m_globals; }
};
