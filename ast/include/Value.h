#ifndef DYNAMO_VALUE_H
#define DYNAMO_VALUE_H

#include <Node.h>

namespace dynamo 
{

enum VALUE_TYPE
{
	VALUE_STRING,
	VALUE_NIL,
	VALUE_NUMBER,
	VALUE_BOOLEAN
};

class Value : public dynamo::Node
{
	std::string m_value;
	VALUE_TYPE m_type;
public:
	Value(VALUE_TYPE type, std::string val): m_value(val), m_type(type) {}
	Value(VALUE_TYPE type, const char* val): m_value(val), m_type(type) {}
	Value(VALUE_TYPE type): m_type(type), m_value("nil") {}
	
	NODE_TYPE getType() const override { return VALUE; }
	std::string getValue() const { return m_value; }
	VALUE_TYPE getValueType() const { return m_type; }
};

}

#endif // DYNAMO_VALUE_H
