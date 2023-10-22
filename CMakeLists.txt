cmake_minimum_required(VERSION 3.26)
project(pose_detection_display)

set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

find_package(OpenCV REQUIRED)
find_package(realsense2 REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Threads)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${realsense2_INCLUDE_DIRS})
include_directories(${EIGEN3_INCLUDE_DIR})

add_subdirectory(src)

target_link_libraries(pose_detection_display ${OpenCV_LIBS} ${realsense2_LIBRARY})
target_link_libraries(pose_detection_display ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(pose_detection_display Eigen3::Eigen)