//
// Created by junchau on 10/23/23.
//

#ifndef BODY_DETECTOR_H
#define BODY_DETECTOR_H

#include "../stdafx.h"
#include "../Task/frame_buffer.h"
#include "../Task/points_buffer.h"

using namespace cv::dnn;

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
    void solve3D(const PointSet &points);

    size_t getPointsCount() const
    {
        return _pointsCount;
    }
private:
    // model parameter structure
#ifndef MODELSET
#define MODELSET
    struct ModelSet
    {
        cv::String dataset;
        cv::String modelTxt;
        cv::String modelBin;
        cv::Mat inputBlob;
        cv::Mat result;
        float thresh;
        float scale;
        int W_in;
        int H_in;
        const int midx = 1, npairs = 14, nparts = 16;
        Net net;
    } _modelSet;
#endif // MODELSET

    const int POSE_PAIRS[20][2] = {
            // MPI body
            {0,1}, {1,2}, {2,3},
            {3,4}, {1,5}, {5,6},
            {6,7}, {1,14}, {14,8}, {8,9},
            {9,10}, {14,11}, {11,12}, {12,13}
    };

    size_t _pointsCount;
    std::array<std::vector<cv::Point>, 2> _bodyPoints;
};


#endif //BODY_DETECTOR_H
