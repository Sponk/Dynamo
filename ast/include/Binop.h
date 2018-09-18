#ifndef DYNAMO_BINOP_H
#define DYNAMO_BINOP_H

#include <Node.h>

namespace dynamo 
{

class Binop : public dynamo::Node
{
	std::string m_operator;
	Node* m_rhs, *m_lhs;
public:
	Binop(const char* op): m_operator(op) {}
	NODE_TYPE getType() const override { return BINOP; }
	
	std::string getOperator() const { return m_operator; }
	void setOperator(const std::string& str) { m_operator = str; }
	
	void setRHS(Node* n) { m_rhs = n; }
	void setLHS(Node* n) { m_lhs = n; }
	
	Node* getRHS() { return m_rhs; }
	Node* getLHS() { return m_lhs; }
};

}

#endif // DYNAMO_BINOP_H
