cmake_minimum_required(VERSION 3.15)
set (CMAKE_CXX_STANDARD 11)
project(shape_based_matching)


# debug or release
SET(CMAKE_BUILD_TYPE "Debug")


# arm or x86
IF(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
    SET(PLATFORM_COMPILE_FLAGS "-mfpu=neon")
ELSE()
    SET(PLATFORM_COMPILE_FLAGS "-march=native")

    # some places of the algorithm are designed for 128 SIMD
    # so 128 SSE may slightly faster than 256 AVX, you may want this
#    SET(PLATFORM_COMPILE_FLAGS "-msse -msse2 -msse3 -msse4 -mssse3")  # SSE only
ENDIF()

# SET(PLATFORM_COMPILE_FLAGS "-DMIPP_NO_INTRINSICS")  # close SIMD
#原本的值
#SET(COMMON_COMPILE_FLAGS "-fopenmp -Wall -Wno-sign-compare")
#修改的值
SET(COMMON_COMPILE_FLAGS "-fopenmp -Wall")

#SET(CMAKE_CXX_FLAGS "${PLATFORM_COMPILE_FLAGS} ${COMMON_COMPILE_FLAGS} $ENV{CXXFLAGS}")
#message("PLATFORM_COMPILE_FLAGS : ${}")
#SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g2 -ggdb")
#SET(CMAKE_CXX_FLAGS_RELEASE "-O3")

#set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} /opt/ros/kinetic)
#find_package(OpenCV 3 REQUIRED)
#include_directories(${OpenCV_INCLUDE_DIRS})


#--------------------------------------------------指定目录-----------------------------------------------------#

# include MIPP headers
#include_directories (${INCLUDE_DIRECTORIES} "${CMAKE_CURRENT_SOURCE_DIR}/MIPP/")
set(MIPP_INCLUDEPATH ${CMAKE_SOURCE_DIR}/MIPP/)
set(LINE2DUP_INCLUDEPATH ${CMAKE_SOURCE_DIR}/line2Dup)
#包含外部脚本
include(cmake/opencv.cmake)
include(cmake/cuda.cmake)

#综合库目录
list(APPEND EXTERN_LIBPATH ${OPENCV_LIBPATH} )
list(APPEND EXTERN_LIBNAME ${OPENCV_LIBNAME} )
message("in main now ${CMAKE_SOURCE_DIR}")

#链接外部库
#target_link_directories(${PROJECT_NAME}_test PUBLIC ${EXTERN_LIBPATH}) #PUBLIC/PRIVATE参数缺失会报错
#target_link_libraries(${PROJECT_NAME}_test ${EXTERN_LIBNAME})

#target_link_libraries(${PROJECT_NAME}_test ${OpenCV_LIBS})

#设置子目录生成路径
set(EXE_OUTPUTPATH ${CMAKE_CURRENT_SOURCE_DIR}/x64/bin)
set(LIB_OUTPUTPATH ${CMAKE_CURRENT_SOURCE_DIR}/x64/lib)

set(LINE2DUP_NAME line2dup)
set(EXAMPLE_NAME example)
set(ORIG_NAME orig_test)

#添加子目录
#库目录
add_subdirectory(CUDA)
add_subdirectory(line2Dup)
#exe目录
add_subdirectory(example)
add_subdirectory(orig_test)