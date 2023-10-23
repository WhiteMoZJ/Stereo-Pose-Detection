//
// Created by junchau on 10/18/23.
//

#include "camera.h"

device::Camera::Camera()
{
    _width = 0;
    _height = 0;
    _frameCount = 0;
}

void device::Camera::setVideoFormat(size_t width, size_t height)
{
    if (_width == width && _height == height)
    {
        return;
    }
    _width = width;
    _height = height;
}

//bool Camera::setExposureTime(int t)
//{
//    return false;
//}

bool device::Camera::setUpStream(size_t fps)
{
    std::cout << "Setting up camera...";
    _frameCount = 0;
    cfg.enable_stream(RS2_STREAM_INFRARED, 1, _width, _height, RS2_FORMAT_Y8, fps);
    cfg.enable_stream(RS2_STREAM_INFRARED, 2, _width, _height, RS2_FORMAT_Y8, fps);

    // disable emitter
    rs2::pipeline_profile selection = pipe.start(cfg);
    rs2::device selected_device = selection.get_device();
    if (!selected_device)
        return false;
    auto depth_sensor = selected_device.first<rs2::depth_sensor>();
    if (depth_sensor.supports(RS2_OPTION_EMITTER_ENABLED))
        depth_sensor.set_option(RS2_OPTION_EMITTER_ENABLED, 0.f);

    std::cout << "Done" << std::endl;
    return true;
}

bool device::Camera::startStream()
{
    ++_frameCount;
    rs2::frameset frameset = pipe.wait_for_frames();

    // get left and right infrared frames from frameset
    rs2::video_frame ir_frame_left = frameset.get_infrared_frame(1);
    rs2::video_frame ir_frame_right = frameset.get_infrared_frame(2);

    left = cv::Mat(cv::Size(_width, _height), CV_8UC1, (void*)ir_frame_left.get_data());
    right = cv::Mat(cv::Size(_width, _height), CV_8UC1, (void*)ir_frame_right.get_data());

    return true;
}

bool device::Camera::endStream()
{
    pipe.stop();
    _frameCount = 0;
    std::cout << "Stream end" << std::endl;
    return false;
}

cv::Size device::Camera::getResolution() const
{
    return cv::Size(_width, _height);
}




