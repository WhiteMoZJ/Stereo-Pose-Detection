//
// Created by junchau on 10/18/23.
//

#include "camera.h"

Camera::Camera()
{
    _fps            = 0;    // auto
    _frameCount     = 0;
    _isStreamOpen   = false;
    _settings.setResolution(640, 480);
}

// !TODO: hot-plugging
bool Camera::setUpStream()
{
    _frameCount = 0;
    _selectedDevice = nullptr;

    // disable emitter
    try {
        _cfg.enable_stream(RS2_STREAM_INFRARED, 1,
            _settings.getResolution().width, _settings.getResolution().height, RS2_FORMAT_Y8, _fps);
        _cfg.enable_stream(RS2_STREAM_INFRARED, 2,
            _settings.getResolution().width, _settings.getResolution().height, RS2_FORMAT_Y8, _fps);

        _selection = _pipe.start(_cfg);
        _selectedDevice = _selection.get_device();

        const auto depth_sensor = _selectedDevice.first<rs2::depth_sensor>();
        if (depth_sensor.supports(RS2_OPTION_EMITTER_ENABLED))
            depth_sensor.set_option(RS2_OPTION_EMITTER_ENABLED, 0.f);
        _isStreamOpen = true;
    }
    catch(const rs2::camera_disconnected_error&) {
        std::cerr << "ERROR: Device Disconnected" << std::endl;
    }
    catch (const rs2::recoverable_error&) {
        std::cerr << "ERROR: Operation Failed, Please Try Again" << std::endl;
    }
    catch (const rs2::error&) {
        std::cerr << "ERROR: Some Other Error Occurred" << std::endl;
    }

    _settings.cameraName = _selectedDevice.get_info(RS2_CAMERA_INFO_NAME);
    _settings.firmwareVersion = _selectedDevice.get_info(RS2_CAMERA_INFO_FIRMWARE_VERSION);
    _settings.serialNum = _selectedDevice.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);

    return _isStreamOpen;
}

bool Camera::startStream()
{
    const rs2::frameset frameset = _pipe.wait_for_frames();
    if (!frameset) {
        _isStreamOpen = false;
        return false;
    }
    // get left and right infrared frames from frameset
    ir_frame_left = frameset.get_infrared_frame(1);
    ir_frame_right = frameset.get_infrared_frame(2);

    ++_frameCount;
    return true;
}

bool Camera::endStream()
{
    // cv::destroyAllWindows();
    _pipe.stop();
    _isStreamOpen = false;
    _frameCount = 0;
    return true;
}

Camera &Camera::operator >> (std::array<cv::Mat, 2> &imgs)
{
    cv::Mat(_settings.getResolution(), CV_8UC1, const_cast<void*>(ir_frame_left.get_data())).copyTo(imgs[0]);
    cv::Mat(_settings.getResolution(), CV_8UC1, const_cast<void*>(ir_frame_right.get_data())).copyTo(imgs[1]);
    return *this;
}




