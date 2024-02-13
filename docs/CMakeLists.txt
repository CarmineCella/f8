cmake_minimum_required(VERSION 3.0)

project(makeref)

SET(CMAKE_CXX_FLAGS "-Wall -O2 -std=c++11 -I . -Wunused-function")

set(binary_SOURCES makeref.cpp)

link_directories(
	${PROJECT_SOURCE_DIR}
)

add_executable(makeref ${binary_SOURCES})

set_target_properties(makeref PROPERTIES LINK_FLAGS "-Wl,-rpath,./")

target_include_directories(makeref
    PUBLIC
		  ${PROJECT_BINARY_DIR}
    	   ${PROJECT_SOURCE_DIR}
 )

# rebuild documentation
add_custom_command(TARGET makeref POST_BUILD
	COMMENT "Rebuilding documentation..."
	COMMAND $<TARGET_FILE:makeref> ../../docs/reference.md ../../src/f8.h ../../src/core.h ../../src/system.h ../../src/signals.h ../../src/plotting.h ../../src/stdlib.tcl
)
