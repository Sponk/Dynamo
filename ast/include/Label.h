#ifndef DYNAMO_LABEL_H
#define DYNAMO_LABEL_H

#include "Node.h"

namespace dynamo 
{

class Label : public dynamo::Node
{
	std::string m_name;
public:
	Label(const char* name): m_name(name) {}
	virtual dynamo::NODE_TYPE getType() const { return LABEL; }
	
	std::string getName() { return m_name; }
};

}

#endif // DYNAMO_LABEL_H
