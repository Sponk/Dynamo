#ifndef DYNAMO_UNOP_H
#define DYNAMO_UNOP_H

#include <Node.h>

namespace dynamo 
{

class Unop : public dynamo::Node
{
	std::string m_operation;
	NodeRef m_operand;
public:
	Unop(const std::string& str): m_operation(str) {}
	NODE_TYPE getType() const override { return UNOP; }
	
	void setOperand(NodeRef node) { m_operand = node; }
	NodeRef getOperand() { return m_operand; }
	std::string getOperator() const { return m_operation; }
	void setOperator(const std::string& str) { m_operation = str; }
};

}

#endif // DYNAMO_UNOP_H
