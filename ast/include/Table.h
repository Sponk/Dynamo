#ifndef DYNAMO_TABLE_H
#define DYNAMO_TABLE_H

#include "Node.h"

namespace dynamo 
{

class Table : public dynamo::Node
{
	std::vector<std::pair<NodeRef, NodeRef>> m_entries;
public:
	dynamo::NODE_TYPE getType() const { return TABLE; }

	std::vector<std::pair<NodeRef, NodeRef>>& getEntries() { return m_entries; }
	void addEntry(const std::pair<NodeRef, NodeRef>& entry) { m_entries.push_back(entry); }
	void addEntry(std::pair<NodeRef, NodeRef>&& entry) { m_entries.push_back(std::move(entry)); }
};

}

#endif // DYNAMO_TABLE_H
