set(IMGUI_SRC
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_impl_opengl3.cpp
        ${IMGUI_DIR}/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
)

add_library(PoseDetect SHARED
        Utils/tool.h
        Utils/tool.cpp
        Utils/body_detector.h
        Utils/body_detector.cpp
        Task/frame_buffer.cpp
        Task/frame_buffer.h
        Task/thread_task.cpp
        Task/thread_task.h
        Device/camera.cpp
        Device/camera.h
        Task/utils.h
        ${IMGUI_SRC}
)

#link_directories(${PROJECT_SOURCE_DIR}/lib)
target_link_libraries(PoseDetect ${OpenCV_LIBS} GL glfw Eigen3::Eigen ${realsense2_LIBRARY})

add_executable(stereo_pose_detection
        main.cpp
)

target_precompile_headers(stereo_pose_detection PUBLIC stdafx.h)
target_link_libraries(stereo_pose_detection ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(stereo_pose_detection PoseDetect)