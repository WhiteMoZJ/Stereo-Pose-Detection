cmake_minimum_required(VERSION 3.16)
project(pose_detection_display)

set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")
set(CMAKE_CXX_STANDARD 14)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(MV_DIR ${PROJECT_SOURCE_DIR}/include/MVSDK)

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++14" COMPILER_SUPPORTS_CXX14)
if(COMPILER_SUPPORTS_CXX14)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    add_definitions(-DCOMPILEDWITHC14)
    message(STATUS "Using flag -std=c++14.")
else()
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++14 support. Please use a different C++ compiler.")
endif()

find_package(OpenCV 4.5 QUIET)
if(NOT OpenCV_FOUND)
    message(FATAL_ERROR "OpenCV > 4.5 not found.")
endif()
#find_package(realsense2 REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Threads REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${EIGEN3_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/include)

add_library(PoseDetect SHARED
        src/Utils/tool.h
        src/Utils/tool.cpp
        src/Utils/body_detector.h
        src/Utils/body_detector.cpp
        src/Task/frame_buffer.cpp
        src/Task/frame_buffer.h
)
add_library(PoseCamera SHARED
        ${MV_DIR}/CameraApi.h
        ${MV_DIR}/CameraDefine.h
        ${MV_DIR}/CameraStatus.h
        src/Device/mv_camera.h
        src/Device/mv_camera.cpp
)

link_directories(${PROJECT_SOURCE_DIR}/lib)
target_link_libraries(PoseDetect libMVSDK.so ${OpenCV_LIBS} GL glfw Eigen3::Eigen)
target_link_libraries(PoseCamera libMVSDK.so ${OpenCV_LIBS})

add_subdirectory(src)

target_link_libraries(pose_detection_display ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(pose_detection_display PoseDetect PoseCamera)