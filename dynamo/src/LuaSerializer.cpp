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

using namespace dynamo;

namespace
{
void dump(Node* node, std::ostream& out)
{
	if(!node)
		return;
	
	switch(node->getType())
	{
		case dynamo::BLOCK:
		{
			Block* b = reinterpret_cast<Block*>(node);
			for(auto n : b->getChildren())
			{
				dump(n, out);
			}
		}
		break;
		case dynamo::ASSIGNMENT:
		{
			Assignment* a = reinterpret_cast<Assignment*>(node);
			
			if(a->isLocal())
				out << "local ";
			
			dump(a->getLHS(), out);
			out << " = ";
			dump(a->getRHS(), out);
			out << " ";
		}
		break;
		
		case dynamo::VARIABLE:
		{
			Variable* v = reinterpret_cast<Variable*>(node);
			
			out << v->getName();
			if(v->getAccessor())
			{
				if(v->getAccessor()->getStaticAccess())
				{
					out << ".";
				}
				else
				{
					out << ":";
				}
				
				dump(v->getAccessor(), out);
			}
		}
		break;
		
		case dynamo::VALUE:
		{
			Value* v = reinterpret_cast<Value*>(node);
			out << v->getValue(); // FIXME Check if value type and format are correct for Lua!
		}
		break;
		
		case dynamo::BINOP:
		{
			Binop* b = reinterpret_cast<Binop*>(node);
			
			out << "(";
			dump(b->getLHS(), out);
			out << " " << b->getOperator() << " ";
			dump(b->getRHS(), out);
			out << ")";
		}
		break;
		
		case dynamo::UNOP:
		{
			Unop* u = reinterpret_cast<Unop*>(node);
			out << u->getOperator() << " ";
			dump(u->getOperand(), out);
		}
		break;
		
		case dynamo::FUNCTION_DECL:
		{
			FunctionDecl* f = reinterpret_cast<FunctionDecl*>(node);
			out << (f->isLocal() ? "local " : "") << "function ";
			dump(f->getName(), out);
			
			out << "(";
			for(auto& param : f->getArguments())
				out << param << (&param != &f->getArguments().back() ? ", " : "");
			out << ")\n";
			
			dump(f->getBody(), out);
			out << "end";
			out << "\n";
		}
		break;
		
		case dynamo::RETURN:
		{
			out << "return ";
			dump(node->getAccessor(), out);
			out << "\n";
		}
		break;
		
		case dynamo::BREAK:
		{
			out << "break\n";
		}
		break;
		
		case dynamo::LABEL:
		{
			Label* l = reinterpret_cast<Label*>(node);
			out << "::" << l->getName() << "::\n";
		}
		break;
		
		case dynamo::GOTO:
		{
			Goto* g = reinterpret_cast<Goto*>(node);
			out << "goto " << g->getName() << "\n";
		}
		break;
		
		case dynamo::FUNCTION_CALL:
		{
			FunctionCall* f = reinterpret_cast<FunctionCall*>(node);
			dump(node->getAccessor(), out);
			out << "(";
			for(auto& param : f->getParameters())
			{
				dump(param, out);
				out << (&param != &f->getParameters().back() ? ", " : "");
			}
			out << ")";
		}
		break;
		
		case dynamo::WHILE:
		{
			While* w = reinterpret_cast<While*>(node);
			if(w->isDoWhile())
			{
				out << "repeat\n";
				dump(w->getBody(), out);
				out << "until ";
				dump(w->getCheck(), out);
			}
			else
			{
				out << "while ";
				dump(w->getCheck(), out);
				out << " do\n";
				dump(w->getBody(), out);
				out << "end";
			}
			out << "\n";
		}
		break;
		
		case dynamo::FOR:
		{
			For* f = reinterpret_cast<For*>(node);
			out << "for ";
			dump(f->getInit(), out);
			out << ", ";
			dump(f->getCheck(), out);
			
			if(f->getIncrement())
			{
				out << ", ";
				dump(f->getIncrement(), out);
			}
			
			out << " do\n";
			dump(f->getBody(), out);
			out << "end";
			out << "\n";
		}
		break;
		
		case dynamo::IF:
		{
			If* ifNode = reinterpret_cast<If*>(node);
			
			if(ifNode->getCheck())
			{
				out << "if ";
				dump(ifNode->getCheck(), out);
				out << " then\n";
			}
			
			dump(ifNode->getBody(), out);
			if(ifNode->getElse())
			{
				out << "\nelse\n";
				dump(ifNode->getElse(), out);
			}
			
			if(ifNode->getCheck())
				out << "end\n";
		}
		break;
		
		case dynamo::TABLE:
		{
			Table* table = reinterpret_cast<Table*>(node);
			size_t counter = 0;
			
			out << "{\n";
			
			for(auto& n : table->getEntries())
			{
				if(n.first == nullptr)
				{
					out << "[" << ++counter << "] = ";
				}
				else
				{
					if(n.first->getType() == VARIABLE)
					{
						dump(n.first, out);
						out << " = ";
					}
					else
					{
						out << "[";
						dump(n.first, out);
						out << "] = ";
					}
				}
				
				dump(n.second, out);
				
				if(&n != &table->getEntries().back())
					out << ",\n";
				else
					out << "\n";
			}
			
			out << "}\n";
		}
		break;
		
		default:
			out << "-- Unknown node!\n";
	}
}
}

void luaSerialize(const Module& module, std::ostream& out)
{
	for(auto& k : module.getNodes())
	{
		dump(k.get(), out);
		out << "\n";
	}
	
	out.flush();
}
