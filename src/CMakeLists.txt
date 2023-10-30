set(IMGUI_DIR imgui)
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
        Device/camera.cpp
        Device/camera.h
        Task/thread_task.cpp
        Task/thread_task.h
        Task/frame_buffer.cpp
        Task/frame_buffer.h
        Utils/tool.cpp
        Utils/tool.h
        Task/utils.h
        Utils/body_detector.h
        Utils/body_detector.cpp
        Task/points_buffer.cpp
        Task/points_buffer.h
        ${IMGUI_SRC}
)

target_precompile_headers(pose_detection_display PUBLIC stdafx.h)