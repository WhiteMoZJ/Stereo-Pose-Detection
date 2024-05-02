set(IMGUI_SRC
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_impl_opengl3.cpp
        ${IMGUI_DIR}/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
)

add_library(Interface SHARED
        ${IMGUI_SRC}
        Utils/tool.h
        Utils/tool.cpp
        Task/frame_buffer.cpp
        Task/frame_buffer.h
        Task/thread_task.cpp
        Task/thread_task.h
        Device/camera.cpp
        Device/camera.h
        Task/utils.h
        Utils/pointset_buffer.cpp
        Utils/pointset_buffer.h
)
target_link_libraries(Interface ${OpenCV_LIBS} GL glfw ${realsense2_LIBRARY})

# CUDA based library
add_library(Detector SHARED Utils/body_detector.h Utils/body_detector.cpp)
set_target_properties(Detector PROPERTIES CUDA_ARCHITECTURES 86)
target_link_libraries(Detector ${OpenCV_LIBS} Eigen3::Eigen)
target_compile_features(Detector PUBLIC cxx_std_14)

add_executable(stereo_pose_detection main.cpp)

target_precompile_headers(stereo_pose_detection PUBLIC stdafx.h)
target_link_libraries(stereo_pose_detection ${CMAKE_THREAD_LIBS_INIT})

set_property(TARGET stereo_pose_detection PROPERTY CUDA_SEPARABLE_COMPILATION ON)
target_link_libraries(stereo_pose_detection Interface Detector)