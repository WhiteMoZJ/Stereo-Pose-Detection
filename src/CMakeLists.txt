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
)

target_precompile_headers(pose_detection_display PUBLIC stdafx.h)