cmake_minimum_required(VERSION 3.15)
set (CMAKE_CXX_STANDARD 11)
project(line2Dup)


# SET(PLATFORM_COMPILE_FLAGS "-DMIPP_NO_INTRINSICS")  # close SIMD
#原本的值
#SET(COMMON_COMPILE_FLAGS "-fopenmp -Wall -Wno-sign-compare")
#修改的值
#SET(COMMON_COMPILE_FLAGS "-fopenmp -Wall")

#SET(CMAKE_CXX_FLAGS "${PLATFORM_COMPILE_FLAGS} ${COMMON_COMPILE_FLAGS} $ENV{CXXFLAGS}")
#SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g2 -ggdb")
#SET(CMAKE_CXX_FLAGS_RELEASE "-O3")

#set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} /opt/ros/kinetic)

aux_source_directory(./ SRC)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${OPENCV_INCLUDEPATH} ${MIPP_INCLUDEPATH})
message("now in  line2dup ${}")
#设置子目录生成路径
set(LIBRARY_OUTPUT_PATH ${LIB_OUTPUTPATH})
add_library(${LINE2DUP_NAME} STATIC ${SRC})
target_link_directories(${LINE2DUP_NAME} PUBLIC ${OPENCV_LIBPATH})
target_link_libraries(${LINE2DUP_NAME} ${OPENCV_LIBNAME})