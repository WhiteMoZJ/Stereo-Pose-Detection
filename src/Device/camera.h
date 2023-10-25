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

    rs2::video_frame ir_frame_left = rs2::video_frame(rs2::frame());
    rs2::video_frame ir_frame_right = rs2::video_frame(rs2::frame());


    /*
     * @brief   Initiate video frame
     * @param   width and height
     */
    void setVideoFormat(size_t width = 640, size_t height = 480);
    void setExposureTime(float t = 0.f);

    /*
     * @brief   Initiate camera
     * @param   frames per second
     * @return  Is camera start successful
     */
    bool setUpStream(size_t fps = 90);

    // video stream control
    /*
     * @brief   Start camera streaming
     * @return  Is streaming
     */
    bool startStream();

    /*
     * @brief   End camera streaming
     * @return  Is streaming
     */
    bool endStream();

    /*
     * @brief   Get resolution
     * @return  cv::Size(_width, _height)
     */
    cv::Size getResolution() const
    {
        return cv::Size(_width, _height);
    }

    /*
     * @brief   Get frame count
     * @return  _frameCount(private number variable)
     */
    unsigned int getFrameCount() const
    {
        return _frameCount;
    }

    /*
     * @brief   Operator overloading
     * @param   cv::Mat array
     * Transfer Camera frames to Frame images
     */
    Camera& operator >> (std::array<cv::Mat, 2> &imgs);

    float fov[2];   // X Y fov

    cv::Mat cam_mat;                // camera matrix
    cv::Mat dis_coeff;              // distortion coefficients

private:
    rs2::config cfg;                // realsense config
    rs2::pipeline pipe;             // realsense pipeline

    unsigned int _width, _height;   // frame size
    unsigned int _frameCount;       // count of frame
    float _exposureTime;            // exposure time



};
}

#endif //CAMERA_H
