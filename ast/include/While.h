#ifndef DYNAMO_WHILE_H
#define DYNAMO_WHILE_H

#include <Node.h>

namespace dynamo 
{

class While : public dynamo::Node
{
	Node* m_body, *m_check;
	bool m_doWhile = false;
	
public:
	NODE_TYPE getType() const override { return WHILE; }
	
	void setBody(Node* n) { m_body = n; }
	void setCheck(Node* n) { m_check = n; }
	
	Node* getBody() { return m_body; }
	Node* getCheck() { return m_check; }
	
	bool isDoWhile() const { return m_doWhile; }
	void setDoWhile(bool b) { m_doWhile = b; }
};

}

#endif // DYNAMO_WHILE_H
