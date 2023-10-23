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

    void setVideoFormat(size_t width = 640, size_t height = 480);
    // bool setExposureTime(int t = 0);
    bool setUpStream(size_t fps = 90);

    // video stream control
    bool startStream();
    bool endStream();


    cv::Size getResolution() const;
    unsigned int getFrameCount() const
    {
        return _frameCount;
    }

private:
    rs2::config cfg;    // realsense config
    rs2::pipeline pipe; // realsense pipeline

    unsigned int _width, _height;
    unsigned int _frameCount;

};
}

#endif //CAMERA_H
