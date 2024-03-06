set(CMAKE_MINIMUM_REQUIRED_VERSION 3.15)

set(CUDA_VERSION "11.0")

enable_language(CUDA)

set(CUDA_ARCHITECTURES "70;75")


set(CUBLAS_HINTS
  ${CUDA_ROOT}
  $ENV{CUDA_ROOT}
  $ENV{CUDA_TOOLKIT_ROOT_DIR}
)
set(CUBLAS_PATHS
  /usr
  /usr/local
  /usr/local/cuda
)

set(CUDA_LIB_NAMES
cublas cublasLt cuda cudadevrt cudart 
cudart_static cudnn cudnn_adv_infer 
cudnn_adv_train cudnn_cnn_infer64_8 
cudnn_cnn_train cudnn_ops_infer 
cudnn_ops_infer64_8 cudnn_ops_train 
cudnn_ops_train64_8 cufftw cufilt 
curand cusolverMg cusparse nppc 
nppial nppicc nppidei nppif nppig 
nppim nppist nppisu nppitc npps 
nvblas nvinfer nvinfer_dispatch 
nvinfer_lean nvinfer_plugin nvinfer_vc_plugin 
nvjpeg nvml nvonnxparser nvparsers 
nvptxcompiler_static nvrtc nvrtc_static 
nvrtc-builtins_static OpenCL
)

# Finds the include directories
find_path(CUBLAS_INCLUDE_DIRS
  NAMES cublas_v2.h cuda.h
  HINTS ${CUBLAS_HINTS}
  PATH_SUFFIXES include inc include/x86_64 include/x64
  PATHS ${CUBLAS_PATHS}
  DOC "cuBLAS include header cublas_v2.h"
)
message("CUBLAS_INCLUDE_DIRS ${CUBLAS_INCLUDE_DIRS}")
mark_as_advanced(CUBLAS_INCLUDE_DIRS)

#Find where the libs
find_path(CUDA_LIB_DIRS
  NAMES cublas.lib
  HINTS ${CUBLAS_HINTS}
  PATH_SUFFIXES lib lib64 lib/x86_64 lib/x64 lib/x86 lib/Win32 lib/import lib64/import
  PATHS ${CUBLAS_PATHS}
  DOC "CUDA library"
)
message("CUDA_LIB_DIRS ${CUDA_LIB_DIRS}")

# Finds the libraries
find_library(CUDA_LIBRARIES
  NAMES cudart cuda cublas
  HINTS ${CUBLAS_HINTS}
  PATH_SUFFIXES lib lib64 lib/x86_64 lib/x64 lib/x86 lib/Win32 lib/import lib64/import
  PATHS ${CUBLAS_PATHS}
  DOC "CUDA library"
)

mark_as_advanced(CUDA_LIBRARIES)

find_library(CUBLAS_LIBRARIES
  NAMES cublas
  HINTS ${CUBLAS_HINTS}
  PATH_SUFFIXES lib lib64 lib/x86_64 lib/x64 lib/x86 lib/Win32 lib/import lib64/import
  PATHS ${CUBLAS_PATHS}
  DOC "cuBLAS library"
)
mark_as_advanced(CUBLAS_LIBRARIES)
# ==================================================================================================

# Notification messages
if(NOT CUBLAS_INCLUDE_DIRS)
    message(STATUS "Could NOT find 'cuBLAS.h', install CUDA/cuBLAS or set CUDA_ROOT")
endif()
if(NOT CUDA_LIBRARIES)
    message(STATUS "Could NOT find CUDA library, install it or set CUDA_ROOT")
endif()
if(NOT CUBLAS_LIBRARIES)
    message(STATUS "Could NOT find cuBLAS library, install it or set CUDA_ROOT")
endif()

# Determines whether or not cuBLAS was found
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(cuBLAS DEFAULT_MSG CUBLAS_INCLUDE_DIRS CUDA_LIBRARIES CUBLAS_LIBRARIES)
