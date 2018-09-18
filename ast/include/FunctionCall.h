#ifndef DYNAMO_FUNCTIONCALL_H
#define DYNAMO_FUNCTIONCALL_H

#include <Node.h>

namespace dynamo 
{

class FunctionCall : public dynamo::Node
{
	std::vector<Node*> m_parameters;
public:
	NODE_TYPE getType() const override { return FUNCTION_CALL; }
	
	void addParameter(Node* n) { m_parameters.push_back(n); }
	std::vector<Node*>& getParameters() { return m_parameters; }
};

}

#endif // DYNAMO_FUNCTIONCALL_H
