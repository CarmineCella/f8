project (f8)

SET(CMAKE_CXX_FLAGS "-Wall -O3 -std=c++14 -I . -Wno-return-type-c-linkage")
add_executable(f8 f8.cpp f8.h core.h system.h signals.h learning.h system/csv_tools.h signals/FFT.h signals/BPF.h signals/FIRDesigner.h signals/features.h signals/WavFile.h plotting.h plotting/svg_tools.h learning/algorithms.h learning/Matrix.h learning/PCA.h)
target_link_libraries (f8 dl ${LIBS})

INSTALL(PROGRAMS stdlib.tcl system.tcl signals.tcl learning.tcl plotting.tcl DESTINATION $ENV{HOME}/.f8)
INSTALL(
    TARGETS f8 
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION $ENV{HOME}/.f8
    ARCHIVE DESTINATION $ENV{HOME}/.f8)

# add_custom_command(TARGET f8 POST_BUILD
# 	COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/../../build/bin/
#     COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:f8> ${PROJECT_SOURCE_DIR}/../../build/bin/
# )

