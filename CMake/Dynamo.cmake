
find_package(BoehmGC)

function(add_lua_module)
	cmake_parse_arguments(ARG "EXE" "NAME" "SOURCES" ${ARGN})
	set(_OUT ${CMAKE_CURRENT_BINARY_DIR}/${ARG_NAME}.cpp)
	
	add_custom_target(${ARG_NAME}-dynamo SOURCES ${ARG_SOURCES} COMMENT "Compile Lua module ${ARG_NAME}")
	add_custom_command(TARGET ${ARG_NAME}-dynamo
			PRE_BUILD
			COMMAND rm -f ${_OUT}
	)
	
	foreach(FILE ${ARG_SOURCES})
		get_filename_component(_WD ${CMAKE_CURRENT_SOURCE_DIR}/${FILE} DIRECTORY)

		add_custom_command(
			TARGET ${ARG_NAME}-dynamo
			COMMAND $<TARGET_FILE:dynamo> ${CMAKE_CURRENT_SOURCE_DIR}/${FILE} | clang-format >> ${_OUT}
			COMMENT "Compiling Lua file ${FILE}"
			WORKING_DIRECTORY ${_WD}
		)
	endforeach()
	
	if(ARG_EXE)
		file(TOUCH ${_OUT})
	
		add_executable(${ARG_NAME} ${_OUT})
		add_dependencies(${ARG_NAME} ${ARG_NAME}-dynamo)
		
		target_include_directories(${ARG_NAME} PRIVATE 
			${CMAKE_SOURCE_DIR}/runtimes/cpp 
			${CMAKE_SOURCE_DIR}/backends/lua/stdlib 
			${CMAKE_CURRENT_BINARY_DIR})
		
		list(REVERSE ARG_SOURCES)
		list(GET ARG_SOURCES 0 MAIN_FILE)
		
		get_filename_component(MAIN_FILE_NAME ${MAIN_FILE} NAME)
		
		string(REPLACE "." "_" MAIN_NAME ${MAIN_FILE_NAME})
		target_compile_definitions(${ARG_NAME} PUBLIC -D${MAIN_NAME}_MAIN)
		
		if(BOEHM_GC_FOUND)
			target_compile_definitions(${ARG_NAME} PUBLIC -DDYNAMO_GC)
			target_link_libraries(${ARG_NAME} ${BOEHM_GC_LIBRARIES})
			target_include_directories(${ARG_NAME} PUBLIC ${BOEHM_GC_INCLUDE_DIR})
		endif()

	endif()
endfunction()

#macro(add_lua_module name file)
#	message("-- LUAMODULE ${name}")
#	add_custom_target(${name} ALL SOURCES ${file} 
#		COMMAND $<TARGET_FILE:dynamo> ${CMAKE_CURRENT_SOURCE_DIR}/${file} > ${CMAKE_CURRENT_BINARY_DIR}/${name}.cpp
#		COMMENT "Compiling Lua module ${file}")
#endmacro()

