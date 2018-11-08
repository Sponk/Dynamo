#ifndef DYNAMO_FUNCTIONDECL_H
#define DYNAMO_FUNCTIONDECL_H

#include <Node.h>

namespace dynamo 
{

class FunctionDecl : public dynamo::Node
{
	NodeRef m_body;
	std::vector<std::string> m_arguments;
	NodeRef m_name;
	bool m_local = false;
	bool m_vararg = false;
	
public:
	NODE_TYPE getType() const override { return FUNCTION_DECL; }
	
	void setBody(NodeRef b) { m_body = b; }
	NodeRef getBody() { return m_body; }
	
	void setArguments(const std::vector<std::string>& v) { m_arguments = v; }
	void setArguments(std::vector<std::string>&& v) { m_arguments = std::move(v); }
	std::vector<std::string>& getArguments() { return m_arguments; }
	
	void setName(NodeRef name) { m_name = name; }
	NodeRef getName() const { return m_name; }
	
	void setLocal(bool v) { m_local = v; }
	bool isLocal() const { return m_local; }
	
	void setVararg(bool v) { m_vararg = v; }
	bool isVararg() const { return m_vararg; }
};

}

#endif // DYNAMO_FUNCTIONDECL_H
