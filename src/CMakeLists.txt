cmake_minimum_required(VERSION 2.8.9)
project (f8)

SET(CMAKE_CXX_FLAGS "-Wall -g -O2 -std=c++17 -I . -Wno-return-type-c-linkage")

option(ENABLE_READLINE "Enable readline shell" OFF)
if (ENABLE_READLINE) 
    set (LIBS ${LIBS} readline) 
    add_definitions (-DENABLE_READLINE)
endif()

add_executable(f8 f8.cpp f8.h)
target_link_libraries (f8 dl ${LIBS})

INSTALL(FILES f8.h DESTINATION include)
INSTALL(PROGRAMS core.scm DESTINATION $ENV{HOME}/.f8)
INSTALL(
    TARGETS f8 
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION $ENV{HOME}/.f8
    ARCHIVE DESTINATION $ENV{HOME}/.f8)
