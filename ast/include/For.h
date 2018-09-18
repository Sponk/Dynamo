#ifndef DYNAMO_FOR_H
#define DYNAMO_FOR_H

#include <Node.h>

namespace dynamo 
{

class For : public dynamo::Node
{
	NodeRef m_body, m_check, m_init, m_increment;
public:
	For(): m_body(nullptr), m_check(nullptr), m_init(nullptr), m_increment(nullptr) {}
	
	NODE_TYPE getType() const override { return FOR; }
	
	void setBody(NodeRef n) { m_body = n; }
	void setCheck(NodeRef n) { m_check = n; }
	void setInit(NodeRef n) { m_init = n; }
	void setIncrement(NodeRef n) { m_increment = n; }

	NodeRef getBody() { return m_body; }
	NodeRef getCheck() { return m_check; }
	NodeRef getInit() { return m_init; }
	NodeRef getIncrement() { return m_increment; }
};

}

#endif // DYNAMO_FOR_H
