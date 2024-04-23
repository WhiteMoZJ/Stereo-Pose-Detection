set(IMGUI_SRC
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_impl_opengl3.cpp
        ${IMGUI_DIR}/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui_tables.cpp

)

add_executable(stereo_pose_detection
        main.cpp
        Task/thread_task.cpp
        Task/thread_task.h
        Device/camera.cpp
        Device/camera.h
        Task/utils.h
        ${IMGUI_SRC}
)

target_precompile_headers(stereo_pose_detection PUBLIC stdafx.h)