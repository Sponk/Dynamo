#ifndef DYNAMO_ASSIGNMENT_H
#define DYNAMO_ASSIGNMENT_H

#include <Node.h>

namespace dynamo 
{

class Assignment : public dynamo::Node
{
	NodeRef lhs, rhs;
	bool m_local = false;
public:
	Assignment(NodeRef lhs, NodeRef rhs, bool local = false):
		lhs(lhs), rhs(rhs), m_local(local) {}
		
	Assignment() {}
		
	NodeRef getLHS() { return lhs; }
	NodeRef getRHS() { return rhs; }
	
	void setLHS(NodeRef r) { lhs = r; }
	void setRHS(NodeRef r) { rhs = r; }
	
	bool isLocal() const { return m_local; }
	void setLocal(bool b) { m_local = b; }
	
	NODE_TYPE getType() const override { return ASSIGNMENT; }
};

}

#endif // DYNAMO_ASSIGNMENT_H
