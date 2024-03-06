cmake_minimum_required(VERSION 3.15)

set(CXX_STANDARD 14)

project(example)
aux_source_directory(./src SRC)

include_directories(${OPENCV_INCLUDEPATH})
link_directories(${OPENCV_LIBPATH})
link_libraries(${OPENCV_LIBNAME})
set(EXECUTABLE_OUTPUT_PATH ${EXE_OUTPUTPATH})
add_executable(${EXAMPLE_NAME} ${SRC})