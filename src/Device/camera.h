//
// Created by junchau on 10/18/23.
//

#ifndef CAMERA_H
#define CAMERA_H

#include <librealsense2/rs.hpp>
#include "../stdafx.h"
#include "../Task/frame_buffer.h"

namespace device
{
class Camera
{
public:
    Camera();
    Camera(const Camera&) = delete;
    ~Camera() = default;

    rs2::video_frame ir_frame_left = rs2::video_frame(rs2::frame());
    rs2::video_frame ir_frame_right = rs2::video_frame(rs2::frame());


    /**
     * @brief   Initiate video frame
     * @param   width and height
     */
    void setVideoFormat(int width = 640, int height = 480, int fps = 90);
    void setExposureTime(float t = 0.f);

    /**
     * @brief   Initiate camera
     * @param   frames per second
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
     * @brief   Get resolution
     * @return  cv::Size(_width, _height)
     */
    cv::Size getResolution() const
    {
        return cv::Size(_width, _height);
    }

    /**
     * @brief   Get frame count
     * @return  _frameCount(private number variable)
     */
    size_t getFrameCount() const
    {
        return _frameCount;
    }

    void printInfo();

    /**
     * @brief   Operator overloading

     Transfer Camera frames to Frame images
     * @param   imgs cv::Mat array
     */
    Camera& operator >> (std::array<cv::Mat, 2> &imgs);

    float fov[2]{79.144, 63.5818};   // X Y fov

    cv::Mat cam_mat;                // camera matrix
    cv::Mat dis_coeff;              // distortion coefficients

private:
    rs2::config _cfg;                // realsense config
    rs2::pipeline _pipe;             // realsense pipeline

    rs2::device _selected_device;
    rs2::pipeline_profile _selection;

    int _width, _height;   // frame size
    unsigned int _frameCount;       // count of frame
    float _exposureTime;            // exposure time

    int _fps;

};
}

#endif //CAMERA_H
