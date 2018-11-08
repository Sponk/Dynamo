#include <Node.h>
#include <Serializer.h>
#include <Assignment.h>
#include <Variable.h>
#include <Value.h>
#include <Binop.h>
#include <Unop.h>
#include <FunctionDecl.h>
#include <While.h>
#include <For.h>
#include <FunctionCall.h>
#include <If.h>
#include <Table.h>
#include <Label.h>
#include <Goto.h>

#include "FindGlobalsPass.hpp"
#include <PassManager.h>
#include <VariableScope.h>

#include <sstream>
#include <algorithm>

using namespace dynamo;

namespace
{
void dump(NodeRef node, VariableScope& scope, std::ostream& out)
{
	if(!node)
		return;
	
	switch(node->getType())
	{
		case dynamo::BLOCK:
		{
			Block* b = reinterpret_cast<Block*>(node.get());
			
			if(b->hasLocalScope())
			{
				out << "{\n";
				scope.push();
			}
			
			for(auto n : b->getChildren())
			{
				dump(n, scope, out);
				out << ";\n";
			}
		
			if(b->hasLocalScope())
			{
				out << "}\n";
				scope.pop();
			}
		}
		break;
		case dynamo::ASSIGNMENT:
		{
			auto a = reinterpret_cast<Assignment*>(node.get());
			
			if(a->getLHS()->getType() == dynamo::VARIABLE)
			{
				auto var = std::reinterpret_pointer_cast<Variable>(a->getLHS());
				// When we access a variable which is defined as local -> Create it in the current scope!
				// If it does not exist yet -> Create it as well!
				if(a->isLocal() || !scope.exists(var->getName()))
				{
					out << "DynamoRuntime::IFixedValue ";
					scope.set(var->getName(), var);
				}
			}
			
			dump(a->getLHS(), scope, out);
			out << " = DynamoRuntime::IFixedValue::makeValue(";
			dump(a->getRHS(), scope, out);
			out << ")";
		}
		break;
		
		case dynamo::VARIABLE:
		{
			auto v = std::reinterpret_pointer_cast<Variable>(node);
			
			if(!scope.exists(v->getName()))
			{
				std::cerr << "Warning: Using global " << v->getName() << std::endl;
				scope.getGlobals()[v->getName()] = v; // TODO Print warning if it does not exist yet!
			}
			
			std::string clean = v->getName();
			
			// External modules might have path stuff in their name
			std::replace(clean.begin(), clean.end(), '/', '_');
			std::replace(clean.begin(), clean.end(), '\\', '_');
			
			out << clean;
			if(v->getAccessor())
			{
				out << "[";
					dump(v->getAccessor(), scope, out);
					out << "]";
			}
		}
		break;
		
		case dynamo::VALUE:
		{
			Value* v = reinterpret_cast<Value*>(node.get());
			out << "DynamoRuntime::IFixedValue::makeValue(" << v->getValue() << ")"; // FIXME Check if value type and format are correct for Lua!
		}
		break;
		
		case dynamo::BINOP:
		{
			Binop* b = reinterpret_cast<Binop*>(node.get());
			
			// Special case for exponentiation
			if(b->getOperator() == "**")
			{
				out << "DynamoRuntime::IFixedValue::makeValue(pow(";
				dump(b->getLHS(), scope, out);
				out << ".get<double>(), ";
				dump(b->getRHS(), scope, out);
				out << ".get<double>()))";
				break;
			}
			
			out << "(";
			dump(b->getLHS(), scope, out);
			out << " " << b->getOperator() << " ";
			dump(b->getRHS(), scope, out);
			out << ")";
		}
		break;
		
		case dynamo::UNOP:
		{
			Unop* u = reinterpret_cast<Unop*>(node.get());
			out << u->getOperator() << " ";
			dump(u->getOperand(), scope, out);
		}
		break;
		
		case dynamo::FUNCTION_DECL:
		{
			scope.push();
			FunctionDecl* f = reinterpret_cast<FunctionDecl*>(node.get());
		
			if(f->getName() != nullptr)
			{
				if(f->getName()->getType() == dynamo::VARIABLE)
				{
					auto var = std::reinterpret_pointer_cast<Variable>(f->getName());
					if(!scope.get(var->getName()))
					{
						out << "DynamoRuntime::IFixedValue ";
						scope.set(var->getName(), var);
					}
				}
				
				dump(f->getName(), scope, out);
				
				out << " = DynamoRuntime::IFixedValue::makeValue(";
			}
			
			out << "std::function<DynamoRuntime::IFixedValue(DynamoRuntime::IFixedValue*, size_t)>(";
			out << "[&] (DynamoRuntime::IFixedValue* __args__, size_t __argnum__) -> DynamoRuntime::IFixedValue {\n";
			
			const auto hasSelf = f->getName() && !f->getName()->getStaticAccess();
			out << "assert(__argnum__ >= " << f->getArguments().size() + (hasSelf ? 1 : 0) << ");\n";
			
			if(f->isVararg())
			{
				out << "DynamoRuntime::IFixedValue* __varargs__ = __args__ + " << f->getArguments().size() << ";\n";
				out << "size_t __varargnum__ = __argnum__ - " << f->getArguments().size() << ";\n";
			}
			
			unsigned short ctr = 0;
			if(hasSelf)
			{
				scope.set("self", nullptr);
				out << "DynamoRuntime::IFixedValue& self = __args__[0];\n";
				ctr++;
			}
			
			for(auto& param : f->getArguments())
			{
				scope.set(param, nullptr);
				out << "DynamoRuntime::IFixedValue& " << param << " = __args__[" << ctr++ << "];\n";
			}
			
			scope.push();
			dump(f->getBody(), scope, out);
			out << "\nreturn DynamoRuntime::IFixedValue();\n})\n";
			
			if(f->getName())
				out << ")";
			
			scope.pop();

		}
		break;
		
		case dynamo::RETURN:
		{
			out << "return ";
			dump(node->getAccessor(), scope, out);
		}
		break;
		
		case dynamo::BREAK:
		{
			out << "break";
		}
		break;
		
		case dynamo::LABEL:
		{
			Label* l = reinterpret_cast<Label*>(node.get());
			out << l->getName() << ":\n";
		}
		break;
		
		case dynamo::GOTO:
		{
			Goto* g = reinterpret_cast<Goto*>(node.get());
			out << "goto " << g->getName() << "";
		}
		break;
		
		case dynamo::FUNCTION_CALL:
		{
			auto f = std::reinterpret_pointer_cast<FunctionCall>(node);
			dump(node->getAccessor(), scope, out);
			out << "(";
			if(f->getAccessor() && !f->getAccessor()->getLastStaticAccess())
			{
				// We don't need the last access since it accesses our function.
				NodeRef lastAccessor = f;
				NodeRef reallyLast;
				while(lastAccessor->getAccessor() && lastAccessor->getAccessor()->getAccessor()) lastAccessor = lastAccessor->getAccessor();
				reallyLast = lastAccessor->getAccessor();
				lastAccessor->setAccessor(nullptr);
				
				dump(node->getAccessor(), scope, out);
				out << (f->getParameters().empty() ? "" : ", ");
				
				lastAccessor->setAccessor(reallyLast);
			}
			
			for(auto& param : f->getParameters())
			{
				dump(param, scope, out);
				out << (&param != &f->getParameters().back() ? ", " : "");
			}
			out << ")";
		}
		break;
		
		case dynamo::WHILE:
		{
			While* w = reinterpret_cast<While*>(node.get());
			scope.push();

			if(w->isDoWhile())
			{
				out << "do\n{\n";
				dump(w->getBody(), scope, out);
				out << "} while(!(";
				dump(w->getCheck(), scope, out);
				out << "))";
			}
			else
			{
				out << "while(!!";
				dump(w->getCheck(), scope, out);
				out << ")\n{\n";
				dump(w->getBody(), scope, out);
				out << "}";
			}
			out << "\n";
			scope.pop();
		}
		break;
		
		case dynamo::FOR:
		{
			For* f = reinterpret_cast<For*>(node.get());
			
			scope.push();

			out << "for(";
			dump(f->getInit(), scope, out);
			out << "; !!";
			dump(f->getCheck(), scope, out);
			
			out << "; ";
			
			dump(f->getIncrement(), scope, out);
			
			out << ")\n{\n";
			
			dump(f->getBody(), scope, out);
			scope.pop();

			out << "\n}";
			out << "\n";
		}
		break;
		
		case dynamo::IF:
		{
			If* ifNode = reinterpret_cast<If*>(node.get());
			
			if(ifNode->getCheck())
			{
				out << "if(!!";
				dump(ifNode->getCheck(), scope, out);
				out << ")\n{\n";
			}
			
			scope.push();
			dump(ifNode->getBody(), scope, out);
			scope.pop();

			if(ifNode->getElse())
			{
				out << "\n}\nelse\n{\n";
				scope.push();
				dump(ifNode->getElse(), scope, out);
				scope.pop();
			}
			
			if(ifNode->getCheck())
				out << "\n}\n";
		}
		break;
		
		case dynamo::TABLE:
		{
			Table* table = reinterpret_cast<Table*>(node.get());
			size_t counter = 0;
			
			if(table->getEntries().empty())
			{
				out << "DynamoRuntime::IFixedValue::makeValue(DynamoRuntime::Table())";
				break;
			}
			else if(table->getEntries().size() == 1 
				&& reinterpret_cast<Variable*>(table->getEntries().front().second.get())->getName() == "...")
			{
				out << "DynamoRuntime::buildTableFromArray(__varargs__, __varargnum__)";
				break;
			}
			
			out << "DynamoRuntime::IFixedValue::makeValue({\n";
			
			for(auto& n : table->getEntries())
			{
				out << "{ ";
				if(n.first == nullptr)
				{
					out << "DynamoRuntime::calculateTableIndex(DynamoRuntime::IFixedValue::makeValue(" << ++counter << ".0)), ";
				}
				else
				{
					dump(n.first, scope, out);
					out << ", ";
				}
				
				dump(n.second, scope, out);
				
				if(&n != &table->getEntries().back())
					out << "},\n";
				else
					out << "}\n";
			}
			
			out << "})";
		}
		break;
		
		default:
			out << "-- Unknown node!\n";
	}
}
}

#define __DYNAMO_COMPILER__
#include <DynamoStdlib.hpp>

void cppSerialize(const Module& module, std::ostream& out)
{
	PassManager pm;
	VariableScope scope;
	
	DynamoStdlib::registerGlobals(scope);
	
	std::stringstream ss;

	ss << "#ifdef " << module.getName() << "_DEFINED\n#warning \"Module function was already defined!\"\n#else\n"; 
	ss << "DynamoRuntime::IFixedValue " << module.getName() << " = DynamoRuntime::IFixedValue::makeValue(std::function<DynamoRuntime::IFixedValue()> ([]()\n{\n";
	
	for(auto& k : module.getNodes())
	{
		dump(k, scope, ss);
		ss << ";\n";
	}
	ss << "return DynamoRuntime::IFixedValue::makeValue(0);\n}));\n";
	ss << "#define " << module.getName() << "_DEFINED\n";
	ss << "#endif\n";


	if(module.isMainModule())
	{
		ss << "#ifdef DYNAMO_SCRIPT_MAIN\n";
		ss << "#define scriptMain " << module.getName() << "\n";
		ss << "#else\n";
		ss << "int main()\n{\nDynamoRuntime::IFixedValue result = " << module.getName() << "();\n"
			<< "if(result.isa<int>()) return result.get<int>(); else if(result.isa<float>()) return (int) result.get<float>(); else if(result.isa<double>()) return (int) result.get<double>();\n return 1;}\n";
		ss << "#endif\n";
	}
	
	// Write to file!
	out << "// Generated by Dynamo v0.1\n";
	out << "#include <DynamoRuntime.hpp>\n";
	out << "#include <DynamoStdlib.hpp>\n";
	
	for(auto node : scope.getGlobals())
		if(node.second)
		{
			std::string clean = node.first;
			std::replace(clean.begin(), clean.end(), '/', '_');
			
			out << "#ifndef " << clean << "_DEFINED\n";
			if(node.second->getType() == VARIABLE && reinterpret_cast<Variable*>(node.second.get())->isExternal())
			{
				out << "#error \"External module " << node.first << " not found!\"\n";
			}
			else
			{
				
				out << "DynamoRuntime::IFixedValue " << clean << ";\n";
				out << "#define " << clean << "_DEFINED\n";
			}
			out << "#endif\n";
		}
	
	out << ss.rdbuf();
	out.flush();
}
