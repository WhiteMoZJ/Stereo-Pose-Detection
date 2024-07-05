//
// Created by junchau on 10/18/23.
//

/*
 * The camera can be connected after the program is started.
 * The camera can be disconnected while the program is running, but cannot be reconnected.
 * !TODO: hot-plugging
 */

#include "camera.h"

Camera::Camera()
{
    _fps            = 0;    // auto
    _frameCount     = 0;
    _isStreamOpen   = false;
    _settings.setResolution(640, 480);
    _isMonitoring = true;
}

Camera::~Camera()
{
    _deviceMonitorThread = std::thread(&Camera::deviceMonitor, this);
    _isMonitoring = false;
    if (_deviceMonitorThread.joinable()) {
        _deviceMonitorThread.join();
    }
}


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
    catch (const rs2::error&) {
        std::cerr << "ERROR: Some Other Error Occurred" << std::endl;
        _isStreamOpen = false;
    }

    _settings.cameraName = _selectedDevice.get_info(RS2_CAMERA_INFO_NAME);
    _settings.firmwareVersion = _selectedDevice.get_info(RS2_CAMERA_INFO_FIRMWARE_VERSION);
    _settings.serialNum = _selectedDevice.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);

    return _isStreamOpen;
}

bool Camera::startStream()
{
    if (!_isStreamOpen) return false;
    const rs2::frameset frameset = _pipe.wait_for_frames();
    _isStreamOpen = frameset ? true : false;
    _isMonitoring = _isStreamOpen;
    if (frameset) {

        // get left and right infrared frames from frameset
        ir_frame_left = frameset.get_infrared_frame(1);
        ir_frame_right = frameset.get_infrared_frame(2);

        ++_frameCount;
    }
    return _isStreamOpen;
}

bool Camera::endStream()
{
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

/*
 * Monitor the device status
 * If the device is disconnected, try to set up the stream again
 * If the device is reconnected, the stream will not be set up again
 */
void Camera::deviceMonitor()
{
    while (_isMonitoring) {
        if (!_selectedDevice || !_selectedDevice.is<rs2::device>()) {
            // Device is disconnected, try to set up the stream again
            setUpStream();
            std::cerr << "No Device Detected, Reconnecting" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}




