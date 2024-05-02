# Stereo Pose Detection(WIP)
Actually it's unfinished...
<a href="https://wakatime.com/badge/github/WhiteMoZJ/Stereo-Pose-Detection"><img src="https://wakatime.com/badge/github/WhiteMoZJ/Stereo-Pose-Detection.svg" alt="wakatime"></a>
## :link: Links: 
- Project: https://github.com/WhiteMoZJ/Stereo-Pose-Detection 
- Model Download: https://github.com/CMU-Perceptual-Computing-Lab/openpose/tree/master/models/pose/mpi

## :computer: Operating System
Ubuntu 22.04.04

## :electric_plug: Hardware
- Camera: Intel RealSense Depth Camera D435i

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
    │   ├── camera.cpp
    │   └── camera.h
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

## Library based on
- [OpenCV >= 4.5.0(WITH_CUDA=ON)](https://github.com/opencv/opencv)
- [Eigen = 3.4.0](https://eigen.tuxfamily.org)
- [GLFW = 3.3.6](https://www.glfw.org/)
- [Imgui = 1.90.5]()
- [RealSense2SDK = 2.55.1](https://www.intelrealsense.com/sdk-2/)

1600 ~ 2000ms to ~70ms