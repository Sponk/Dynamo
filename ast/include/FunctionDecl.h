#ifndef DYNAMO_FUNCTIONDECL_H
#define DYNAMO_FUNCTIONDECL_H

#include <Node.h>

namespace dynamo 
{

class FunctionDecl : public dynamo::Node
{
	Node* m_body;
	std::vector<std::string> m_arguments;
	Node* m_name;
	bool m_local = false;
	
public:
	NODE_TYPE getType() const override { return FUNCTION_DECL; }
	
	void setBody(Node* b) { m_body = b; }
	Node* getBody() { return m_body; }
	
	void setArguments(const std::vector<std::string>& v) { m_arguments = v; }
	void setArguments(std::vector<std::string>&& v) { m_arguments = std::move(v); }
	std::vector<std::string>& getArguments() { return m_arguments; }
	
	void setName(Node* name) { m_name = name; }
	Node* getName() const { return m_name; }
	
	void setLocal(bool v) { m_local = v; }
	bool isLocal() const { return m_local; }
};

}

#endif // DYNAMO_FUNCTIONDECL_H
