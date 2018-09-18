#ifndef DYNAMO_FOR_H
#define DYNAMO_FOR_H

#include <Node.h>

namespace dynamo 
{

class For : public dynamo::Node
{
	Node* m_body, *m_check, *m_init, *m_increment;
public:
	For(): m_body(nullptr), m_check(nullptr), m_init(nullptr), m_increment(nullptr) {}
	
	NODE_TYPE getType() const override { return FOR; }
	
	void setBody(Node* n) { m_body = n; }
	void setCheck(Node* n) { m_check = n; }
	void setInit(Node* n) { m_init = n; }
	void setIncrement(Node* n) { m_increment = n; }

	Node* getBody() { return m_body; }
	Node* getCheck() { return m_check; }
	Node* getInit() { return m_init; }
	Node* getIncrement() { return m_increment; }
};

}

#endif // DYNAMO_FOR_H
