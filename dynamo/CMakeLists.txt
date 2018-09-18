
add_executable(dynamo src/main.cpp src/LuaSerializer.cpp src/CPPSerializer.cpp)
target_link_libraries(dynamo PUBLIC backend dynamo-ast)
target_include_directories(dynamo PUBLIC src)

## For Lua
target_include_directories(dynamo PUBLIC ../backends/lua/stdlib)

install(TARGETS dynamo RUNTIME DESTINATION bin)

