#include "DynamoRuntime.hpp"
#include <iostream>
#include <chrono>

using namespace DynamoRuntime;

template<typename Result, typename Param>
Result fibonacciS(Param p)
{
	if(p == 0)
		return 0;
	else if(p <= 2)
		return 1;
	
	return fibonacciS<Result>(p - 2) + fibonacciS<Result>(p - 1);
}

IFixedValue fibonacciV(const IFixedValue& p)
{
	if(p == 0)
		return IFixedValue::makeValue(0);
	else if(p <= 2)
		return IFixedValue::makeValue(1);
	return fibonacciV(p - 2) + fibonacciV(p - 1);
}

IFixedValue test(const IFixedValue& p)
{
	if(p == 10)
		return p;
	
	return test(p + 1);
}

int fastonacci(int p)
{
	if(p == 0)
		return 0;
	else if(p <= 2)
		return 1;
	
	return fastonacci(p - 2) + fastonacci(p - 1);
}

int main(int argc, char** argv)
{
	dynamoInit();

	IFixedValue table = IFixedValue::makeValue({
		{ IFixedValue::makeValue(5), IFixedValue::makeValue(5) },
		{ IFixedValue::makeValue(3), IFixedValue::makeValue(5) },
		{ IFixedValue::makeValue(2), IFixedValue::makeValue(5) },
		{ IFixedValue::makeValue(8), IFixedValue::makeValue(5) }
	});

	for(auto& k : *table.get<TableRef>())
		std::cout << k.first.get<int>() << std::endl;
	
	//std::cout << (test(IFixedValue::makeValue(5))).get<int>() << std::endl;
	//return 0;
	const int n = 30;
	
	auto start = std::chrono::high_resolution_clock::now();
	auto value1 = fibonacciV(IFixedValue::makeValue(n));
	auto end = std::chrono::high_resolution_clock::now();
	
	std::chrono::duration<double> elapsed = end - start;
	std::cout << value1.get<int>() << " with " << elapsed.count() << " s" << std::endl;

	auto time1 = elapsed.count();
	
	start = std::chrono::high_resolution_clock::now();
	int v = fastonacci(n);
	end = std::chrono::high_resolution_clock::now();
	
	elapsed = end - start;
	std::cout << v << " with " << elapsed.count() << " s" << std::endl;
	std::cout << "The native version is " << time1/elapsed.count() << "x faster" << std::endl;
	return 0;
}
