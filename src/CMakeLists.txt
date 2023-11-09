set(IMGUI_DIR Imgui)
set(IMGUI_SRC
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_impl_opengl3.cpp
        ${IMGUI_DIR}/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui_tables.cpp

)

add_executable(pose_detection_display
        main.cpp
        Task/thread_task.cpp
        Task/thread_task.h
        Task/utils.h
        ${IMGUI_SRC}
)

target_precompile_headers(pose_detection_display PUBLIC stdafx.h)