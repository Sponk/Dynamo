#ifndef DYNAMO_NODE_H
#define DYNAMO_NODE_H

#include <memory>
#include <vector>
#include <iostream>
namespace dynamo 
{

enum NODE_TYPE
{
	NONE,
	UNOP,
	BINOP,
	ASSIGNMENT,
	FUNCTION_CALL,
	FUNCTION_DECL,
	IF,
	NUMBER,
	VALUE,
	VARIABLE,
	WHILE,
	FOR,
	
	BLOCK,
	RETURN,
	TABLE,
	BREAK,
	LABEL,
	GOTO
};

class Node
{
	Node* m_accessor = nullptr; // Accessor for array accesses btw field accesses
	bool m_staticAccess = true;
public:
	virtual NODE_TYPE getType() const = 0;
	
	void setAccessor(Node* n) { m_accessor = n; }
	Node* getAccessor() { return m_accessor; }
	const Node* getAccessor() const { return m_accessor; }
	
	void setStaticAccess(bool b) { m_staticAccess = b; }
	bool getStaticAccess() const { return m_staticAccess; }
	
	bool getLastStaticAccess() const 
	{
		const Node* iter = this;
		while(iter->getAccessor())
		{
			iter = iter->getAccessor();
		}
		
		return iter->getStaticAccess();
	}
};

typedef std::shared_ptr<Node> NodeRef;

class Block : public Node
{
	std::vector<Node*> m_children;
public:
	
	NODE_TYPE getType() const override { return BLOCK; }
	std::vector<Node*>& getChildren() { return m_children; }
	void addChild(Node* n) { m_children.push_back(n); }
};

}

#endif // DYNAMO_NODE_H
