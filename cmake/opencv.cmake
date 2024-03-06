cmake_minimum_required(VERSION 3.15)
set (CMAKE_CXX_STANDARD 14)

#opencv±¾µØÄ¿Â¼
set(OPENCV_LIBPATH D:/OpenCV480/lib)
list(APPEND OPENCV_INCLUDEPATH D:/OpenCV480/lib/include D:/OpenCV480/lib/include/opencv2)

set(OPENCV_LIBNAME opencv_world480d)


include_directories(${OPENCV_INCLUDEPATH})
