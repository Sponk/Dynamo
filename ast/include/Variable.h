#ifndef DYNAMO_VARIABLE_H
#define DYNAMO_VARIABLE_H

#include "Node.h"

namespace dynamo 
{

class Variable : public dynamo::Node
{
	std::string m_name;
	bool m_external = false; // True if this call is defined in another module.

public:
	Variable(const char* name): m_name(name) {}
	Variable(const std::string& name): m_name(name) {}
	Variable(std::string&& name): m_name(std::move(name)) {}
	
	std::string getName() { return m_name; }
	NODE_TYPE getType() const override { return VARIABLE; }
	
	bool isExternal() const { return m_external; }
	void setExternal(bool b) { m_external = b; }
};

}

#endif // DYNAMO_VARIABLE_H
