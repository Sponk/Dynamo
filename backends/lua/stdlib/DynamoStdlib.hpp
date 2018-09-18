#pragma once

#ifdef __DYNAMO_COMPILER__

namespace DynamoStdlib
{
void registerGlobals(VariableScope& scope)
{
	scope.set("dassert", nullptr);
	scope.set("print", nullptr);
}
}

#else

#include <DynamoRuntime.hpp>
#include <iostream>

#define nil DynamoRuntime::IFixedValue()

DynamoRuntime::IFixedValue DynamoRuntime::calculateTableIndex(const DynamoRuntime::IFixedValue& v)
{
	if(v.isa<int>())
		return v - 1;
	else if(v.isa<double>())
	{
		return v - 1.0;
	}
	else if(v.isa<float>())
		return v - 1.0f;
	
	return v;
}

void dassert(bool b, const DynamoRuntime::IFixedValue& msg)
{
	if(!b)
	{
		std::cerr << "Assertion failed: " << *msg.get<DynamoRuntime::StringRef>() << std::endl;
		exit(1);
	}
}

void dassert(const DynamoRuntime::IFixedValue& b, const DynamoRuntime::IFixedValue& msg)
{
	if(!b.isa<bool>() || !b.get<bool>())
	{
		std::cerr << "Assertion failed: " << *msg.get<DynamoRuntime::StringRef>() << std::endl;
		exit(1);
	}
}

template<typename... Args>
void print(Args&& ...args)
{
	auto arglist = std::vector<DynamoRuntime::IFixedValue>({args...});
	for(auto& msg : arglist)
	{
		if(msg.isa<DynamoRuntime::StringRef>())
			std::cout << *msg.get<DynamoRuntime::StringRef>();
		else if(msg.isa<DynamoRuntime::TableRef>())
			std::cout << "<table " << std::hex << "0x" << msg.get<DynamoRuntime::TableRef>() << ">" << std::dec;
		else if(msg.type >= DynamoRuntime::MAX_TYPE)
			std::cout << "<function " << std::hex << "0x" << *((uintptr_t*) msg.getData()) << ">" << std::dec;
		else
			DynamoRuntime::IFixedValue::doTyped(msg, [&msg](auto value) {
				std::cout << value;
			});
	
		if(&msg != &arglist.back())
			std::cout << " \t";
	}
	
	std::cout << "\n";
}
#endif
