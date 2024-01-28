cmake_minimum_required(VERSION 2.8.9)
project (f8)

SET(CMAKE_CXX_FLAGS "-Wall -g -O2 -std=c++11 -I . -Wno-return-type-c-linkage")
add_executable(f8 f8.cpp f8.h system.h numeric.h FFT.h BPF.h WavFile.h plotting.h svg_tools.h)
target_link_libraries (f8 dl ${LIBS})

INSTALL(FILES f8.h DESTINATION include)
INSTALL(PROGRAMS stdlib.scm DESTINATION $ENV{HOME}/.f8)
INSTALL(
    TARGETS f8 
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION $ENV{HOME}/.f8
    ARCHIVE DESTINATION $ENV{HOME}/.f8)
