#include <iostream>
#include <Serializer.h>
#include <Module.h>

#include <thread>
#include <future>
#include <mutex>

#ifndef VERSION_STRING
#define VERSION_STRING "0.1"
#endif

using namespace dynamo;

std::shared_ptr<Module> parse(const std::string& file, int argc, char** argv);

void cppSerialize(const Module& module, std::ostream& out);
void luaSerialize(const Module& module, std::ostream& out);

static std::mutex s_parseMtx, s_serializeMtx;

int processModule(const std::string& file, int argc, char** argv)
{
	std::shared_ptr<Module> module;
	
	{
		std::lock_guard<std::mutex> lock(s_parseMtx);
		module = parse(file, argc, argv);
	}
	
	if(!module)
		return 1;
	
	std::vector<std::future<int>> futures;
	futures.reserve(module->getImports().size());
	
	for(auto& import : module->getImports())
	{
		futures.push_back(std::async(std::launch::async, processModule, import, argc, argv));
	}
	
	// Here we can do fully parallel stuff!
	// FIXME Ugly!
	module->setMainModule(file == argv[1]);
	
	for(auto& future : futures)
		if(future.get())
			return 1;
	
	{
		std::lock_guard<std::mutex> lock(s_serializeMtx);
		cppSerialize(*module, std::cout);
	}
	return 0;
}

int main(int argc, char **argv)
{
	// std::cout << "Dynamo compiler v" VERSION_STRING << std::endl;
	
	if(argc < 2)
	{
		std::cout << "No file given." << std::endl;
		return 0;
	}

	std::async(std::launch::async, processModule, argv[1], argc, argv).get();
	return 0;
}
