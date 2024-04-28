//
// Created by junchau on 10/23/23.
//

#ifndef BODY_DETECTOR_H
#define BODY_DETECTOR_H

#include "../stdafx.h"
#include "../Task/frame_buffer.h"

using namespace cv::dnn;

struct PointSet
{
    static PointSet& getPoints()
    {
        static PointSet pointset;
        return pointset;
    }

private:
    std::vector<int> points;
};

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

private:

};


#endif //BODY_DETECTOR_H
