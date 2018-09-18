#ifndef DYNAMO_FUNCTIONCALL_H
#define DYNAMO_FUNCTIONCALL_H

#include <Node.h>

namespace dynamo 
{

class FunctionCall : public dynamo::Node
{
	std::vector<NodeRef> m_parameters;
public:
	NODE_TYPE getType() const override { return FUNCTION_CALL; }
	
	void addParameter(NodeRef n) { m_parameters.push_back(n); }
	std::vector<NodeRef>& getParameters() { return m_parameters; }
};

}

#endif // DYNAMO_FUNCTIONCALL_H
