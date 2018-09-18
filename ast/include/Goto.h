#ifndef DYNAMO_GOTO_H
#define DYNAMO_GOTO_H

#include "Node.h"

namespace dynamo 
{

class Goto : public dynamo::Node
{
	std::string m_name;
public:
	Goto(const char* name): m_name(name) {}

	virtual dynamo::NODE_TYPE getType() const { return GOTO; }
	std::string getName() { return m_name; }
};

}

#endif // DYNAMO_GOTO_H
