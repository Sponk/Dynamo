
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

BISON_TARGET(parser src/parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cc)
FLEX_TARGET(lexer src/lexer.l  ${CMAKE_CURRENT_BINARY_DIR}/lexer.cc)
ADD_FLEX_BISON_DEPENDENCY(lexer parser)

add_library(backend
	${BISON_parser_OUTPUTS}
	${FLEX_lexer_OUTPUTS})

target_include_directories(backend PRIVATE src)
target_link_libraries(backend PRIVATE dynamo-ast)

add_lua_module(NAME operators_test SOURCES lua/operators.lua EXE)
add_lua_module(NAME arithmetic_test SOURCES lua/arithmetic.lua EXE)
add_lua_module(NAME forloop_test SOURCES lua/forloop.lua EXE)
add_lua_module(NAME whileloop_test SOURCES lua/whileloop.lua EXE)
add_lua_module(NAME if_test SOURCES lua/if.lua EXE)
add_lua_module(NAME function_test SOURCES lua/function.lua EXE)
add_lua_module(NAME table_test SOURCES lua/table.lua EXE)
add_lua_module(NAME globals_test SOURCES lua/globals.lua EXE)
add_lua_module(NAME benchmark_test SOURCES lua/benchmark.lua EXE)
