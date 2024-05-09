//
// Created by junchau on 10/18/23.
//

#ifndef CAMERA_H
#define CAMERA_H

#include <librealsense2/rs.hpp>
#include "../stdafx.h"

struct CameraSettings
{

    float size = 0.4;
    std::string cameraName{"Unknown"}, firmwareVersion{"Unknown"}, serialNum{"Unknown"};


    static CameraSettings& getSettings()
    {
        static CameraSettings settings;
        return settings;
    }

    /**
     * @brief   Initiate video frame
     * @param   width   video frame width
     * @param   height  video frame height
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
     * @brief   Get resolution
     * @return  cv::Size(_width, _height)
     */
    cv::Size getResolution() const
    {
        return {_width, _height};
    }

    const float fov[2]{87, 58};     // X Y fov
    const float baseline = 55;

private:
    int _width = 0, _height = 0;   // frame size
};

class Camera
{
public:
    Camera();
    Camera(const Camera&) = delete;
    ~Camera() = default;

    /**
     * @brief   Initiate camera
     * @return  Is camera start successful
     */
    bool setUpStream();

    // video stream control
    /**
     * @brief   Start camera streaming
     * @return  Is streaming
     */
    bool startStream();

    /**
     * @brief   End camera streaming
     * @return  Is streaming
     */
    bool endStream();

    /**
     * @brief   Get frame count
     * @return  _frameCount(private number variable)
     */
    size_t getFrameCount() const
    {
        return _frameCount;
    }

    bool isOpened() const
    {
        return _isStreamOpen;
    }

    /**
     * @brief   Operator overloading

     Transfer Camera frames to Frame images
     * @param   imgs cv::Mat array
     */
    Camera& operator >> (std::array<cv::Mat, 2> &imgs);

private:
    rs2::video_frame ir_frame_left = rs2::video_frame(rs2::frame());
    rs2::video_frame ir_frame_right = rs2::video_frame(rs2::frame());

    rs2::config             _cfg;              // realsense config
    rs2::pipeline           _pipe;             // realsense pipeline

    rs2::device             _selectedDevice;
    rs2::pipeline_profile   _selection;
    CameraSettings&         _settings = CameraSettings::getSettings();

    unsigned int            _frameCount;       // count of frame
    int                     _fps;
    bool                    _isStreamOpen;

};

#endif //CAMERA_H
