#ifndef DYNAMO_IF_H
#define DYNAMO_IF_H

#include <Node.h>

namespace dynamo 
{

class If : public dynamo::Node
{
	Node* m_check, *m_body, *m_else;
public:
	NODE_TYPE getType() const override { return IF; }
	
	Node* getCheck() { return m_check; }
	Node* getBody() { return m_body; }
	Node* getElse() { return m_else; }
	
	void setCheck(Node* n) { m_check = n; }
	void setBody(Node* n) { m_body = n; }
	void setElse(Node* n) { m_else = n; }
};

}

#endif // DYNAMO_IF_H
