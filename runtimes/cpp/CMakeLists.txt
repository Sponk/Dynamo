
find_package(BoehmGC REQUIRED)

add_executable(CPPRuntimeTest Test.cpp)

if(BOEHM_GC_FOUND)
	target_compile_definitions(CPPRuntimeTest PUBLIC -DDYNAMO_GC)
	target_link_libraries(CPPRuntimeTest ${BOEHM_GC_LIBRARIES})
	target_include_directories(CPPRuntimeTest PUBLIC ${BOEHM_GC_INCLUDE_DIR})
endif()
