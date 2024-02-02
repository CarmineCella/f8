cmake_minimum_required(VERSION 2.8.9)
project (f8)

SET(CMAKE_CXX_FLAGS "-Wall -g -O2 -std=c++11 -I . -Wno-return-type-c-linkage")
add_executable(f8 f8.cpp f8.h core.h system.h signals.h signals/FFT.h signals/BPF.h signals/WavFile.h plotting.h plotting/svg_tools.h)
target_link_libraries (f8 dl ${LIBS})

INSTALL(PROGRAMS stdlib.tcl DESTINATION $ENV{HOME}/.f8)
INSTALL(
    TARGETS f8 
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION $ENV{HOME}/.f8
    ARCHIVE DESTINATION $ENV{HOME}/.f8)
