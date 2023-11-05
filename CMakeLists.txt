cmake_minimum_required(VERSION 3.16)
project(pose_detection_display)

set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(MV_DIR ./include/MVSDK)

find_package(OpenCV REQUIRED)
#find_package(realsense2 REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Threads REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
#include_directories(${realsense2_INCLUDE_DIRS})
include_directories(${EIGEN3_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/include)

add_library(posesdk
        ${MV_DIR}/CameraApi.h
        ${MV_DIR}/CameraDefine.h
        ${MV_DIR}/CameraStatus.h
        src/Device/mv_camera.h
        src/Device/mv_camera.cpp
)
link_directories(${PROJECT_SOURCE_DIR}/lib)
target_link_libraries(posesdk libMVSDK.so ${OpenCV_LIBS})

add_subdirectory(src)

target_link_libraries(pose_detection_display ${OpenCV_LIBS})
#target_link_libraries(pose_detection_display ${realsense2_LIBRARY})
target_link_libraries(pose_detection_display ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(pose_detection_display Eigen3::Eigen)
target_link_libraries(pose_detection_display GL glfw)
target_link_libraries(pose_detection_display posesdk)