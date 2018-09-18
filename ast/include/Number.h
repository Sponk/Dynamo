#ifndef DYNAMO_NUMBER_H
#define DYNAMO_NUMBER_H

#include <Node.h>

namespace dynamo 
{

class Number : public dynamo::Node
{
	std::string m_value = "0";
public:
	Number(const std::string& value):
		m_value(value) {}
		
	std::string getValue() const { return m_value; }
	NODE_TYPE getType() const override { return NUMBER; }
};

}

#endif // DYNAMO_NUMBER_H
