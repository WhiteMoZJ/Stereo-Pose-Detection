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
        Utils/gui.h
        Utils/gui.cpp
        Utils/frame_buffer.cpp
        Utils/frame_buffer.h
        Device/camera.cpp
        Device/camera.h
)
target_link_libraries(Interface ${OpenCV_LIBS} OpenGL::GL ${CMAKE_DL_LIBS} glfw ${realsense2_LIBRARY} Eigen3::Eigen)

# CUDA based library
add_library(Detector SHARED
        Utils/body_detector.cpp
        Utils/body_detector.h
        Utils/pointset_buffer.cpp
        Utils/pointset_buffer.h
        Utils/kalman_filter.cpp
        Utils/kalman_filter.h
)
set_target_properties(Detector PROPERTIES CUDA_ARCHITECTURES 86)
target_link_libraries(Detector ${OpenCV_LIBS} Eigen3::Eigen)
target_compile_features(Detector PUBLIC cxx_std_14)

add_executable(${PROJECT_NAME}
        main.cpp
        Task/utils.h
        Task/thread_task.cpp
        Task/thread_task.h
)

target_precompile_headers(${PROJECT_NAME} PUBLIC stdafx.h)
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
set_property(TARGET ${PROJECT_NAME} PROPERTY CUDA_SEPARABLE_COMPILATION ON)
target_link_libraries(${PROJECT_NAME} Interface Detector)
