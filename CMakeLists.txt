cmake_minimum_required(VERSION 3.16)
project(stereo_pose_detection)

set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")
set(CMAKE_CXX_STANDARD 14)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++14" COMPILER_SUPPORTS_CXX14)
if(COMPILER_SUPPORTS_CXX14)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    add_definitions(-DCOMPILEDWITHC14)
    message(STATUS "Using flag -std=c++14.")
else()
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++14 support. Please use a different C++ compiler.")
endif()

message("building in ${CMAKE_SYSTEM_NAME}")

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions(-DDEBUG)
endif ()

#set(Eigen3_DIR 3rdparty/Eigen3/share/eigen3/cmake)
#set(OpenCV_DIR 3rdparty/OpenCV)
#set(glfw3_DIR 3rdparty/GLFW/lib/cmake/glfw3)
set(IMGUI_DIR ${PROJECT_SOURCE_DIR}/src/Imgui)

find_package(realsense2 REQUIRED)
message("Found realsense2: Version ${realsense2_VERSION}")
find_package(Eigen3 REQUIRED NO_MODULE)
message("Found Eigen: Version ${Eigen3_VERSION}")
find_package(Threads REQUIRED)
find_package(glfw3 REQUIRED)
message("Found GLFW: Version ${glfw3_VERSION}")
find_package(OpenGL REQUIRED)
find_package(OpenCV 4.5 QUIET)
message("Found OpenCV: Version ${OpenCV_VERSION}")

#include_directories("3rdparty/*/include")

add_subdirectory(src)