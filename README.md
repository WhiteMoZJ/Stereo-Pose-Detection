# Pose Detection and Display
## Operating System
Ubuntu 22.04

## Hardware
- Camera: MindVision MV-SUA133GC-T1V-C
- Lens: MindVision MV-LD-6-4M-G(6mm)
- Board: RoboMaster Development Board Type C

## File tree
```
.
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
    ├── main.cpp
    ├── Device
    │   ├── mv_camera.cpp
    │   └── mv_camera.h
    ├── Imgui(imgui files)
    ├── stdafx.h
    ├── Task
    │   ├── frame_buffer.cpp
    │   ├── frame_buffer.h
    │   ├── thread_task.cpp
    │   ├── thread_task.h
    │   └── utils.h
    └── Utils
        ├── body_detector.cpp
        ├── body_detector.h
        ├── tool.cpp
        └── tool.h
```

## Library needed
- C++14
- OpenCV >= 4.5
- Eigen3
- OpenGL3 & GLFW