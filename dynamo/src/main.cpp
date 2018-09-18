#include <iostream>
#include <Serializer.h>
#include <Module.h>

#ifndef VERSION_STRING
#define VERSION_STRING "0.1"
#endif

using namespace dynamo;

std::shared_ptr<Module> parse(const std::string& file, int argc, char** argv);

void cppSerialize(const Module& module, std::ostream& out);
void luaSerialize(const Module& module, std::ostream& out);

int main(int argc, char **argv)
{
	// std::cout << "Dynamo compiler v" VERSION_STRING << std::endl;
	
	if(argc < 2)
	{
		std::cout << "No file given." << std::endl;
		return 0;
	}
	
	auto module = parse(argv[1], argc-1, argv+1);
	if(!module)
		return 1;
	
	cppSerialize(*module, std::cout);
	return 0;
}
