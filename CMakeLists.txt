cmake_minimum_required(VERSION 2.6)
project(dynamo)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH
	${CMAKE_SOURCE_DIR}/CMake
	${CMAKE_SOURCE_DIR}/CMake/Packages
	${CMAKE_MODULE_PATH})
	
if(ENABLE_SANITIZERS)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address -fsanitize=undefined")
	set(CMAKE_CXX_COMPILE_FLAGS "${CMAKE_CXX_COMPILE_FLAGS} -fsanitize=address -fsanitize=undefined")
endif()
	
enable_testing()
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose)

include(Dynamo)

add_subdirectory(ast)
add_subdirectory(backends)
add_subdirectory(dynamo)
add_subdirectory(runtimes)
