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

class Node;
typedef std::shared_ptr<Node> NodeRef;

class Node
{
	NodeRef m_accessor = nullptr; // Accessor for array accesses btw field accesses
	bool m_staticAccess = true;
public:
	virtual NODE_TYPE getType() const = 0;
	
	void setAccessor(Node* n) { m_accessor = NodeRef(n); }
	void setAccessor(NodeRef n) { m_accessor = n; }
	NodeRef getAccessor() { return m_accessor; }
	const NodeRef getAccessor() const { return m_accessor; }
	
	void setStaticAccess(bool b) { m_staticAccess = b; }
	bool getStaticAccess() const { return m_staticAccess; }
	
	bool getLastStaticAccess() const 
	{
		const Node* iter = this;
		while(iter->getAccessor())
		{
			iter = iter->getAccessor().get();
		}
		
		return iter->getStaticAccess();
	}
};

class Block : public Node
{
	std::vector<NodeRef> m_children;
	bool m_localScope = false;
public:
	
	NODE_TYPE getType() const override { return BLOCK; }
	std::vector<NodeRef>& getChildren() { return m_children; }
	void addChild(NodeRef n) { m_children.push_back(n); }
	void addChild(Node* n) { addChild(NodeRef(n)); }
	void setLocalScope(bool b) { m_localScope = b; }
	bool hasLocalScope() const { return m_localScope; }
};

}

#endif // DYNAMO_NODE_H
