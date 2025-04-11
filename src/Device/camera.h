//
// Created by junchau on 10/18/23.
//

#ifndef CAMERA_H
#define CAMERA_H

#include <librealsense2/rs.hpp>
#include "../stdafx.h"

/**
 * @brief Struct to store camera settings
 */
struct CameraSettings
{
    float size = 0.4;                           // Size of the camera
    std::string cameraName{"Unknown"};          // Name of the camera
    std::string firmwareVersion{"Unknown"};     // Firmware version of the camera
    std::string serialNum{"Unknown"};           // Serial number of the camera

    Eigen::Matrix3f intrinsics;

    /**
     * @brief Get the CameraSettings object
     * 
     * @return CameraSettings& The CameraSettings object
     */
    static CameraSettings& getSettings()
    {
        static CameraSettings settings;
        return settings;
    }

    /**
     * @brief Set the resolution of the camera
     * 
     * @param width The width of the video frame
     * @param height The height of the video frame
     */
    void setResolution(const int width, const int height)
    {
        if (_width == width && _height == height)
        {
            return;
        }
        _width  = width;
        _height = height;
    }

    /**
     * @brief Get the resolution of the camera
     * 
     * @return cv::Size The resolution of the camera
     */
    cv::Size getResolution() const
    {
        return {_width, _height};
    }

    const float baseline = 55.0;  // mm

private:
    int _width = 0, _height = 0;   // frame size
};

// you can define your owen class Camera
// just be sure that providing the intrinsics(required) and other necessary parameters

/**
 * @brief Class representing a camera.
 */
class Camera
{
public:
    Camera();
    Camera(const Camera&) = delete;
    ~Camera();

    /**
     * @brief Set up the camera stream
     * 
     * @return true if the camera stream is set up successfully, false otherwise
     */
    bool setUpStream();

    /**
     * @brief Start the camera streaming
     * 
     * @return true if the camera streaming is started successfully, false otherwise
     */
    bool startStream();

    /**
     * @brief End the camera streaming
     * 
     * @return true if the camera streaming is ended successfully, false otherwise
     */
    bool endStream();

    /**
     * @brief Get the frame count
     * 
     * @return size_t The frame count
     */
    size_t getFrameCount() const
    {
        return _frameCount;
    }

    /**
     * @brief Check if the camera is opened
     * 
     * @return true if the camera is opened, false otherwise
     */
    bool isOpened() const
    {
        return _isStreamOpen;
    }

    /**
     * @brief Operator overloading to transfer camera frames to frame images
     * 
     * @param imgs The cv::Mat array to store the frame images
     * @return Camera& The Camera object
     */
    Camera& operator >> (std::array<cv::Mat, 2> &imgs);

private:
    void deviceMonitor();

    rs2::video_frame        ir_frame_left = rs2::video_frame(rs2::frame());
    rs2::video_frame        ir_frame_right = rs2::video_frame(rs2::frame());

    rs2::config             _cfg;              // realsense config
    rs2::pipeline           _pipe;             // realsense pipeline

    rs2::device             _selectedDevice;
    rs2::pipeline_profile   _selection;
    CameraSettings&         _settings = CameraSettings::getSettings();

    unsigned int            _frameCount;       // count of frame
    int                     _fps;
    bool                    _isStreamOpen;

    std::thread             _deviceMonitorThread;   // thread to monitor device
    std::atomic<bool>       _isMonitoring{};          // flag to monitor device

    bool                    _flag = false;
};

#endif //CAMERA_H
