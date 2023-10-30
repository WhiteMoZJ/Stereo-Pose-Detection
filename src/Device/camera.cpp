//
// Created by junchau on 10/18/23.
//

#include "camera.h"

device::Camera::Camera()
{
    _width          = 0;
    _height         = 0;
    _fps            = 0;
    _frameCount     = 0;
    _exposureTime   = 0.f;
    _open           = false;
}

void device::Camera::setVideoFormat(int width, int height, int fps)
{
    if (_width == width && _height == height && _fps == fps)
    {
        return;
    }
    _width  = width;
    _height = height;
    _fps = fps;
}

void device::Camera::setExposureTime(float t)
{
    if (_exposureTime == t)
    {
        return;
    }
    _exposureTime = t;
}

bool device::Camera::setUpStream()
{
    _frameCount = 0;
    _selected_device = nullptr;
    int failed_count = 0;
    // disable emitter
    while (!_selected_device) {
        if (failed_count == 5)
            // Restart 5 times
            return false;
        try {
            _cfg.enable_stream(RS2_STREAM_INFRARED, 1, _width, _height, RS2_FORMAT_Y8, _fps);
            _cfg.enable_stream(RS2_STREAM_INFRARED, 2, _width, _height, RS2_FORMAT_Y8, _fps);

            _selection = _pipe.start(_cfg);
            _selected_device = _selection.get_device();
            break;
        }
        catch(const rs2::camera_disconnected_error& e) {
            std::cerr << "ERROR: Device Disconnected" << std::endl;
        }
        catch (const rs2::recoverable_error& e)
        {
            std::cerr << "ERROR: Operation Failed, Please Try Again" << std::endl;
        }
        catch (const rs2::error& e)
        {
            std::cerr << "ERROR: Some Other Error Occurred" << std::endl;
        }
        failed_count++;
    }

    auto depth_sensor = _selected_device.first<rs2::depth_sensor>();
    if (depth_sensor.supports(RS2_OPTION_EMITTER_ENABLED))
        depth_sensor.set_option(RS2_OPTION_EMITTER_ENABLED, 0.f);
    _open = true;
    return true;
}

bool device::Camera::startStream()
{
    rs2::frameset frameset = _pipe.wait_for_frames();
    if (!frameset) {
        _open = false;
        return false;
    }
    // get left and right infrared frames from frameset
    ir_frame_left = frameset.get_infrared_frame(1);
    ir_frame_right = frameset.get_infrared_frame(2);

    ++_frameCount;
    return true;
}

bool device::Camera::endStream()
{
    cv::destroyAllWindows();
    _pipe.stop();
    _open = false;
    _frameCount = 0;
    return true;
}

void device::Camera::printInfo()
{
    printf("CAM:\t\t%s\n"
           "FIRMWARE:\t%s\n"
           "SERIAL:\t\t%s\n"
           "FORMAT:\t\t%dx%d/%d\n",
           _selected_device.get_info(RS2_CAMERA_INFO_NAME),
           _selected_device.get_info(RS2_CAMERA_INFO_FIRMWARE_VERSION),
           _selected_device.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER),
           _width, _height, _fps);
}

device::Camera &device::Camera::operator >> (std::array<cv::Mat, 2> &imgs)
{
    imgs[0] = cv::Mat(cv::Size(_width, _height), CV_8UC1, (void*)ir_frame_left.get_data());
    imgs[1] = cv::Mat(cv::Size(_width, _height), CV_8UC1, (void*)ir_frame_right.get_data());
    return *this;
}




