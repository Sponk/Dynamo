#include "VariableScope.h"
#include "Variable.h"

#include <iostream>

using namespace dynamo;

void VariableScope::push()
{
	m_scope.push_back(std::unordered_map<std::string, Node*>());
}

void VariableScope::pop()
{
	m_scope.pop_back();
}
	
Node* VariableScope::get(const std::string& name)
{
	for(auto i = m_scope.rbegin(); i != m_scope.rend(); i++)
	{
		auto node = i->find(name);
		if(node != i->end())
		{
			return node->second;
		}
	}
	
	// If no copy was found, create a global one.
	// auto node = new Variable(name);
	// m_scope.front()[name] = node;
	return nullptr;
}

void VariableScope::set(const std::string& name, Node* n)
{
	m_scope.back()[name] = n;
}

bool VariableScope::exists(const std::string& name) const
{
	for(auto i = m_scope.rbegin(); i != m_scope.rend(); i++)
	{
		auto node = i->find(name);
		if(node != i->end())
		{
			return true;
		}
	}
	
	return false;
}
