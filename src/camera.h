//
// Created by junchau on 10/18/23.
//

#ifndef CAMERA_H
#define CAMERA_H

#include <librealsense2/rs.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>

struct Frame
{
    cv::Mat dMat;
    int timestap;
};

class Camera
{
public:
    int width, height;

    Camera();
    ~Camera() = default;

    Frame frame_left;
    Frame frame_right;

    void startStream();
    void endStream();

private:
    rs2::config cfg;    // realsense config
    rs2::pipeline pipe; // realsense pipe
};


#endif //CAMERA_H
