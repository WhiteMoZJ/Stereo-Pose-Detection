//
// Created by junchau on 10/18/23.
//

#include "camera.h"

device::Camera::Camera()
{
    _width          = 0;
    _height         = 0;
    _frameCount     = 0;
    _exposureTime   = 0.f;
}

void device::Camera::setVideoFormat(size_t width, size_t height)
{
    if (_width == width && _height == height)
    {
        return;
    }
    _width  = width;
    _height = height;
}

void device::Camera::setExposureTime(float t)
{
    if (_exposureTime == t)
    {
        return;
    }
    _exposureTime = t;
}

bool device::Camera::setUpStream(size_t fps)
{
    std::cout << "Setting up Camera...";
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

//    if(_exposureTime != 0.f)
//        selected_device.query_sensors()[1].set_option(RS2_OPTION_EXPOSURE, _exposureTime);
//    else
//        selected_device.query_sensors()[1].set_option(RS2_OPTION_AUTO_EXPOSURE_PRIORITY, true);


    auto depth_stream = selection.get_stream(RS2_STREAM_DEPTH)
            .as<rs2::video_stream_profile>();
    auto i = depth_stream.get_intrinsics();
    rs2_fov(&i, fov);

    std::cout << "Done" << std::endl;
    return true;
}

bool device::Camera::startStream()
{
    ++_frameCount;
    rs2::frameset frameset = pipe.wait_for_frames();

    // get left and right infrared frames from frameset
    ir_frame_left = frameset.get_infrared_frame(1);
    ir_frame_right = frameset.get_infrared_frame(2);

//    auto depth_profile = frameset.get_profile();
//    auto depth_intrin = rs2::video_stream_profile(depth_profile).get_intrinsics();

//    std::cout << depth_intrin.fx << std::endl;
//    std::cout << depth_intrin.fy << std::endl;
//    for (float coeff : depth_intrin.coeffs)
//        std::cout << coeff << std::endl;
    return true;
}

bool device::Camera::endStream()
{
    cv::destroyAllWindows();
    pipe.stop();
    _frameCount = 0;
    return false;
}

device::Camera &device::Camera::operator>>(std::array<cv::Mat, 2> &imgs)
{
    imgs[0] = cv::Mat(cv::Size(_width, _height), CV_8UC1, (void*)ir_frame_left.get_data());
    imgs[1] = cv::Mat(cv::Size(_width, _height), CV_8UC1, (void*)ir_frame_right.get_data());
    return *this;
}




