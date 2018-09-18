#ifndef DYNAMO_VARIABLESCOPE_H
#define DYNAMO_VARIABLESCOPE_H

#include <Node.h>
#include <unordered_map>
#include <vector>

namespace dynamo 
{

class VariableScope
{
	std::vector<std::unordered_map<std::string, Node*>> m_scope;
	
public:
	VariableScope() { push(); }
	~VariableScope() { pop(); }
	
	void push();
	void pop();
	
	Node* get(const std::string& name);
	void set(const std::string& name, Node* n);
	bool exists(const std::string& name) const;
	
	std::unordered_map<std::string, Node*>& getGlobals() { return m_scope[0]; }
};

}

#endif // DYNAMO_VARIABLESCOPE_H
