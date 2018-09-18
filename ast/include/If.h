#ifndef DYNAMO_IF_H
#define DYNAMO_IF_H

#include <Node.h>

namespace dynamo 
{

class If : public dynamo::Node
{
	NodeRef m_check, m_body, m_else;
public:
	NODE_TYPE getType() const override { return IF; }
	
	NodeRef getCheck() { return m_check; }
	NodeRef getBody() { return m_body; }
	NodeRef getElse() { return m_else; }
	
	void setCheck(NodeRef n) { m_check = n; }
	void setBody(NodeRef n) { m_body = n; }
	void setElse(NodeRef n) { m_else = n; }
};

}

#endif // DYNAMO_IF_H
