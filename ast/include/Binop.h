#ifndef DYNAMO_BINOP_H
#define DYNAMO_BINOP_H

#include <Node.h>

namespace dynamo 
{

class Binop : public dynamo::Node
{
	std::string m_operator;
	NodeRef m_rhs, m_lhs;
public:
	Binop(const char* op): m_operator(op) {}
	NODE_TYPE getType() const override { return BINOP; }
	
	std::string getOperator() const { return m_operator; }
	void setOperator(const std::string& str) { m_operator = str; }
	
	void setRHS(NodeRef n) { m_rhs = n; }
	void setLHS(NodeRef n) { m_lhs = n; }
	
	NodeRef getRHS() { return m_rhs; }
	NodeRef getLHS() { return m_lhs; }
};

}

#endif // DYNAMO_BINOP_H
