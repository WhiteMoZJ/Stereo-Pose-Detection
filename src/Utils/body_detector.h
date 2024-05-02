//
// Created by junchau on 10/23/23.
//

#ifndef BODY_DETECTOR_H
#define BODY_DETECTOR_H

#include "../stdafx.h"
#include "../Task/frame_buffer.h"
#include "pointset_buffer.h"
#include "../Device/camera.h"

class BodyDetector
{
public:

    BodyDetector();
    ~BodyDetector()  = default;

    /**
     * @brief   Detect body from frame
     * @param   frame Frame object
     * @return  Detect successful
     */
    bool detectBody(const Frame &frame);

    /**
     * @brief   Solve body points in 3D
     * @param
     */
    void solve3D();

    double getFramerate();

private:
    const std::string modelTxt = "../data/models/pose_deploy_linevec_faster_4_stages.prototxt";
    const std::string modelBin = "../data/models/pose_iter_160000.caffemodel";

    const int POSE_PAIRS[20][2] = { // MPI body
     {0,1}, {1,2}, {2,3},
     {3,4}, {1,5}, {5,6},
     {6,7}, {1,14}, {14,8}, {8,9},
     {9,10}, {14,11}, {11,12}, {12,13}
    };

    std::string dataset = "MPI";
    float thresh = 0.1;
    float scale = 0.003922;
    const int npairs = 14, nparts = 16;
    cv::dnn::Net net;
};


#endif //BODY_DETECTOR_H
