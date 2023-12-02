# Stereo Pose Detection(WIP)
Actually it's unfinished...
<a href="https://wakatime.com/badge/github/WhiteMoZJ/Stereo-Pose-Detection"><img src="https://wakatime.com/badge/github/WhiteMoZJ/Stereo-Pose-Detection.svg" alt="wakatime"></a>
## :link: Links: 
- Project: https://github.com/WhiteMoZJ/Stereo-Pose-Detection 
- Model Download: https://github.com/CMU-Perceptual-Computing-Lab/openpose/tree/master/models/pose/mpi

## :computer: Operating System
Ubuntu 22.04

## :electric_plug: Hardware
- Camera: MindVision MV-SUA133GC-T1V-C
- Lens: MindVision MV-LD-6-4M-G(6mm)
- Board: RoboMaster Development Board Type C

## :page_facing_up: File tree
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
- [OpenCV \>= 4.5](https://github.com/opencv/opencv)
- [Eigen \>= 3.3](https://eigen.tuxfamily.org)
- [OpenGL \>= 3.0](https://www.opengl.org/)
- [GLFW \>= 3.0](https://www.glfw.org/)
