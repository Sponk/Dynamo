#pragma once
#include <Pass.h>
#include <Node.h>
#include <For.h>
#include <Assignment.h>
#include <Binop.h>
#include <vector>

class LuaDofile: public dynamo::Pass
{
	dynamo::Module& m_module;
public:
	LuaDofile(dynamo::Module& m): m_module(m) {}
	void handleNode(dynamo::NodeRef node) override
	{
		if(node->getType() == dynamo::FUNCTION_CALL)
		{
			auto fn = std::reinterpret_pointer_cast<dynamo::FunctionCall>(node);
			auto dofile = std::reinterpret_pointer_cast<dynamo::Variable>(node->getAccessor());
			if(dofile && dofile->getName() == "dofile")
			{
				auto& parameters = fn->getParameters();
				check(parameters.size() > 0, "Dofile was not given any parameters!");
				
				auto filename = std::dynamic_pointer_cast<dynamo::Value>(parameters.front());
				check(filename && filename->getValueType() == VALUE_STRING, "Dofile requires a compile time string to work!");
			
				std::string name = filename->getValue().substr(1, filename->getValue().size() - 2);
				m_module.getImports().push_back(name);
				
				std::replace(name.begin(), name.end(), '.', '_');
				
				auto variable = std::make_shared<Variable>(name);
				variable->setExternal(true); // We need to include it from another module!
				
				fn->setAccessor(variable);
				parameters.clear();
			}
		}
	}
};
