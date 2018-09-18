#ifndef DYNAMO_VARIABLE_H
#define DYNAMO_VARIABLE_H

#include "Node.h"

namespace dynamo 
{

class Variable : public dynamo::Node
{
	std::string m_name;
public:
	Variable(const char* name): m_name(name) {}
	Variable(const std::string& name): m_name(name) {}
	Variable(std::string&& name): m_name(std::move(name)) {}
	
	std::string getName() { return m_name; }
	NODE_TYPE getType() const override { return VARIABLE; }
};

}

#endif // DYNAMO_VARIABLE_H
