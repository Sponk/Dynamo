
set(SOURCES
	src/Node.cpp
	src/Assignment.cpp
	src/Binop.cpp
	src/For.cpp
	src/FunctionCall.cpp
	src/FunctionDecl.cpp
	src/If.cpp
	src/Unop.cpp
	src/While.cpp
	src/Value.cpp
	src/Module.cpp
	src/Variable.cpp
	src/Return.cpp
	src/Table.cpp
	src/PassManager.cpp
	src/Pass.cpp
	src/VariableScope.cpp)

set(INCLUDES
	include/Node.h
	include/Node.h
	include/Assignment.h
	include/Binop.h
	include/For.h
	include/FunctionCall.h
	include/FunctionDecl.h
	include/If.h
	include/Unop.h
	include/While.h
	include/Value.h
	include/Module.h
	include/Variable.h
	include/Return.h
	include/Table.h
	include/Pass.h
	include/PassManager.h
	include/VariableScope.h)

add_library(dynamo-ast ${SOURCES} ${INCLUDES})
target_include_directories(dynamo-ast PUBLIC include)

if(NOT NO_TESTS)
	add_executable(dynamo-ast-test EXCLUDE_FROM_ALL
		test/NodeTest.cpp
	)

	find_package(GTest REQUIRED)
	target_link_libraries(dynamo-ast-test ${GTEST_LIBRARIES} dynamo-ast)
	target_include_directories(dynamo-ast-test PUBLIC ${GTEST_INCLUDE_DIR})

	add_test(NAME dynamo-ast-test COMMAND $<TARGET_FILE:dynamo-ast-test>)
	add_dependencies(check dynamo-ast-test)
endif()
