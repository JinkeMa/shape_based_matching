cmake_minimum_required(VERSION 3.15)

set(CXX_STANDARD 14)

project(example)
aux_source_directory(./src SRC)

include_directories(${OPENCV_INCLUDEPATH} ${JSON_INCLUDEPATH})


set(EXECUTABLE_OUTPUT_PATH ${EXE_OUTPUTPATH})
add_executable(${EXAMPLE_NAME} ${SRC})

link_libraries(${OPENCV_LIBNAME})
target_link_directories(${EXAMPLE_NAME} PUBLIC ${OPENCV_LIBPATH})
target_link_libraries(${EXAMPLE_NAME} PUBLIC ${OPENCV_LIBNAME})