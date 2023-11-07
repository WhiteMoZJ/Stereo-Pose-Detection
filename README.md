# Pose Detection and Display

## Hardware

- Camera: MindVision MV-SUA133GC-T1V-C
- Lens: MindVision MV-LD-6-4M-G(6mm)
- Board: RoboMaster Development Board Type C

## File tree

```
.
├── bin
│   └── pose_detection_display
├── CMakeLists.txt
├── configs
│   └── config.ini
├── data
│   └── models
│       ├── pose_deploy_linevec_faster_4_stages.prototxt
│       ├── pose_deploy_linevec.prototxt
│       └── pose_iter_160000.caffemodel
└── src
    ├── CMakeLists.txt
    ├── Device
    │   ├── mv_camera.cpp
    │   └── mv_camera.h
    ├── gui_config.h
    ├── Imgui(imgui files)
    ├── main.cpp
    ├── stdafx.h
    ├── Task
    │   ├── frame_buffer.cpp
    │   ├── frame_buffer.h
    │   ├── points_buffer.cpp
    │   ├── points_buffer.h
    │   ├── thread_task.cpp
    │   ├── thread_task.h
    │   └── utils.h
    └── Utils
        ├── body_detector.cpp
        ├── body_detector.h
        ├── tool.cpp
        └── tool.h
```