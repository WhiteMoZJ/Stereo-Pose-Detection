//
// Created by junchau on 10/23/23.
//

#ifndef BODY_DETECTOR_H
#define BODY_DETECTOR_H

#include "../stdafx.h"
#include "frame_buffer.h"
#include "pointset_buffer.h"
#include "../Device/camera.h"
// #include "kalman_filter.h"

/**
 * @brief Class for detecting and solving body points in 3D.
 */
class BodyDetector
{
public:

    BodyDetector();
    ~BodyDetector()  = default;

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
    static float getFramerate()
    {
        return _framerate;
    };

private:
    /**
     * @brief Solves the body points in 3D.
     * @return The 3D space points of the body.
     */
    SpacePoints solve3D(PointArray& points) const;

    const std::string modelTxt = "../data/models/pose_deploy_linevec_faster_4_stages.prototxt";
    const std::string modelBin = "../data/models/pose_iter_160000.caffemodel";

    const int POSE_PAIRS[14][2] = { // MPI body
     {0,1}, {1,2}, {2,3},
     {3,4}, {1,5}, {5,6},
     {6,7}, {1,14}, {14,8}, {8,9},
     {9,10}, {14,11}, {11,12}, {12,13}
    };

    float thresh = 0.1;
    float scale = 0.003922;
    const int npairs = 14, nparts = 16;
    cv::dnn::Net net;
    static float _framerate;

    CameraSettings& _cameraSettings = CameraSettings::getSettings();
    // std::unique_ptr<KalmanFilter> _kalman_filter;

};


#endif //BODY_DETECTOR_H
