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
    void detectBody(const Frame &frame, PointSet& points);
    void solve3D(const PointSet &points);
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
};


#endif //BODY_DETECTOR_H
