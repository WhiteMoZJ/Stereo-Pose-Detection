//
// Created by junchau on 10/23/23.
//

#ifndef BODY_DETECTOR_H
#define BODY_DETECTOR_H

#include "../stdafx.h"
#include "../Task/frame_buffer.h"

using namespace cv::dnn;

// TODO: HyperPose is under consideration

class BodyDetector
{
public:

    BodyDetector();
    ~BodyDetector()  = default;

    /**
     * @brief   Detect body from frame
     * @param   frame Frame object
     * @param   points PointSet object
     * @return  Detect successful
     */
    bool detectBody(const Frame &frame);

    /**
     * @brief   Solve body points in 3D
     * @param
     */
    void solve3D();

private:

};


#endif //BODY_DETECTOR_H
