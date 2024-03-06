cmake_minimum_required(VERSION 3.15)
set (CMAKE_CXX_STANDARD 11)
project(orig_test)


# include MIPP headers
include_directories (${LINE2DUP_INCLUDEPATH} ${INCLUDE_DIRECTORIES} ${MIPP_INCLUDEPATH})
message("now in orig ../")

aux_source_directory(./src SRC)
aux_source_directory(../ SRC2)
set(EXECUTABLE_OUTPUT_PATH ${EXE_OUTPUTPATH})
# test exe
add_executable(${ORIG_NAME} ${SRC} ${SRC2})

#链接外部库
target_link_directories(${ORIG_NAME} PUBLIC ${OPENCV_LIBPATH} ${LIB_OUTPUTPATH}) #PUBLIC/PRIVATE参数缺失会报错
target_link_libraries(${ORIG_NAME} ${OPENCV_LIBNAME} ${LINE2DUP_NAME})