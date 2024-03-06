
cmake_minimum_required(VERSION 3.15)

project(cuda_lib LANGUAGES CUDA CXX)

#include(${CMAKE_SOURCE_DIR}/cmake/cuda.cmake)

message("CUBLAS_INCLUDE_DIRS: ${CUBLAS_INCLUDE_DIRS}")
message("CUDA LIBRARIES: ${CUDA_LIBRARIES}")

aux_source_directory(./ SRC)
#include_directories(${CUBLAS_INCLUDE_DIRS})
#link_directories(${CUDA_LIB_DIRS})

#target
#add_library(cuda_lib STATIC ${SRC})
add_executable(cuda_exe ${SRC})
#set(LIBRARY_OUTPUT_PATH ${LIB_OUTPUTPATH})
set(EXECUTABLE_OUTPUT_PATH ${EXE_OUTPUTPATH})
set_target_properties(cuda_exe PROPERTIES CUDA_ARCHITECTURES "70")
target_include_directories(cuda_exe PUBLIC ${CUBLAS_INCLUDE_DIRS})
target_link_directories(cuda_exe PRIVATE ${CUDA_LIB_DIRS} ${OPENCV_LIBPATH})
target_link_libraries(cuda_exe ${CUDA_LIB_NAMES} ${OPENCV_LIBNAME})