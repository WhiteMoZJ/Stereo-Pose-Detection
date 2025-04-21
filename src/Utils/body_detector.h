//
// Created by junchau on 10/23/23.
//

#ifndef BODY_DETECTOR_H
#define BODY_DETECTOR_H

#ifdef DEBUG
#include <iostream>
#include <fstream>
#endif

#include "../stdafx.h"
#include "frame_buffer.h"
#include "pointset_buffer.h"
#include "../Device/camera.h"
#include "kalman_filter.h"

/**
 * @brief Class for detecting and solving body points in 3D.
 */
class BodyDetector
{
public:

    BodyDetector();
    ~BodyDetector();

    /**
     * @brief Detects the body from a frame.
     * @param frame The Frame object.
     * @return True if the body is successfully detected, false otherwise.
     */
    SpacePoints detectBody(const Frame &frame);

    /**
     * @brief Gets the framerate.
     * @return The framerate.
     */
    float getFramerate() const
    {
        return _framerate;
    };

private:
    /**
     * @brief Solves the body points in 3D.
     * @return The 3D space points of the body.
     */
    bool solve3D(const PointArray& points);

    const std::string modelTxt = "../data/models/pose_deploy_linevec.prototxt";
    const std::string modelBin = "../data/models/pose_iter_160000.caffemodel";

    float thresh = 0.12;
    float scale = 0.003922;
    const int nparts = 15;
    cv::dnn::Net net;

    float _framerate;
    long _frameCount = 0;

    CameraSettings& _cameraSettings = CameraSettings::getSettings();
    std::unique_ptr<KalmanFilter> _kalman_filter[2][16];
    std::unique_ptr<KalmanFilter> _kalman_filter_z[16];

    SpacePoints _spacePoints;
    PointArray _current_point_array;

    bool filter_flag = false;
#ifdef DEBUG
    std::ofstream outfile_filter = std::ofstream("filter.txt");
    std::ofstream outfile_raw = std::ofstream("raw.txt");
#endif
};


#endif //BODY_DETECTOR_H
