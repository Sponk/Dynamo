#ifndef DYNAMO_MODULE_H
#define DYNAMO_MODULE_H

#include <vector>
#include <Node.h>

namespace dynamo 
{
class Module
{
	std::vector<NodeRef> m_nodes;
	std::string m_name = "unnamed";
	std::vector<std::string> m_imports;
	
	bool m_mainModule = false;
	
public:
	void addNode(const NodeRef& ref)
	{
		m_nodes.push_back(ref);
	}
	
	const std::vector<NodeRef>& getNodes() const { return m_nodes; }
	
	std::string getName() const { return m_name; }
	void setName(const std::string& str) { m_name = str; }
	
	std::vector<std::string>& getImports() { return m_imports; }
	
	bool isMainModule() const { return m_mainModule; }
	void setMainModule(bool b) { m_mainModule = b; }
};

}

#endif // DYNAMO_MODULE_H
