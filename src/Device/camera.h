//
// Created by junchau on 10/18/23.
//

#ifndef CAMERA_H
#define CAMERA_H

#include <librealsense2/rs.hpp>
#include "../stdafx.h"

namespace device
{
class Camera
{
public:
    Camera();
    Camera(const Camera&) = delete;
    ~Camera() = default;

    cv::Mat left;
    cv::Mat right;

    unsigned int fps;

    bool setUp();
    // bool setExposureTime(int t = 0);
    // bool setFps(int fps);
    void setVideoFormat(size_t width, size_t height);

    // video stream control
    bool startStream();
    bool endStream();

private:
    rs2::config cfg;    // realsense config
    rs2::pipeline pipe; // realsense pipeline

    unsigned int _width, _height;
    unsigned int _framecount;


    // realsence video frame
    rs2::video_frame ir_frame_left = rs2::video_frame(rs2::frame());
    rs2::video_frame ir_frame_right = rs2::video_frame(rs2::frame());
};
}

#endif //CAMERA_H
