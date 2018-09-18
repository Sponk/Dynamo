#ifndef DYNAMO_MODULE_H
#define DYNAMO_MODULE_H

#include <vector>
#include <Node.h>

namespace dynamo 
{
class Module
{
	std::vector<NodeRef> m_nodes;
	
public:
	void addNode(const NodeRef& ref)
	{
		m_nodes.push_back(ref);
	}
	
	const std::vector<NodeRef>& getNodes() const { return m_nodes; }
};

}

#endif // DYNAMO_MODULE_H
