#ifndef DYNAMO_ASSIGNMENT_H
#define DYNAMO_ASSIGNMENT_H

#include <Node.h>

namespace dynamo 
{

class Assignment : public dynamo::Node
{
	Node* lhs, *rhs;
	bool m_local = false;
public:
	Assignment(Node* lhs, Node* rhs, bool local = false):
		lhs(lhs), rhs(rhs), m_local(local) {}
		
	Assignment() {}
		
	Node* getLHS() { return lhs; }
	Node* getRHS() { return rhs; }
	
	void setLHS(Node* r) { lhs = r; }
	void setRHS(Node* r) { rhs = r; }
	
	bool isLocal() const { return m_local; }
	void setLocal(bool b) { m_local = b; }
	
	NODE_TYPE getType() const override { return ASSIGNMENT; }
};

}

#endif // DYNAMO_ASSIGNMENT_H
