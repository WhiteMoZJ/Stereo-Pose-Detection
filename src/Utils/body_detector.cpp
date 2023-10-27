//
// Created by junchau on 10/23/23.
//

#include "body_detector.h"

BodyDetector::BodyDetector()
{
    _pointsCount = 0;

    _modelSet.dataset = "MPI";
    // The file path relate to binary execute file
    // about 200MB to load openpose model
    _modelSet.modelTxt = cv::samples::findFile("../data/models/pose_deploy_linevec_faster_4_stages.prototxt");
    _modelSet.modelBin = cv::samples::findFile("../data/models/pose_iter_160000.caffemodel");

    _modelSet.thresh = 0.08;
    _modelSet.scale  = 0.003;
    _modelSet.W_in = 120;
    _modelSet.H_in = 160;

    if (_modelSet.modelTxt.empty() || _modelSet.modelBin.empty())
    {
        std::cout << "A sample app to demonstrate human or hand pose detection with a pretrained OpenPose dnn." << std::endl;
        exit(-100);
    }

    // read the network model
    _modelSet.net = readNet(_modelSet.modelBin, _modelSet.modelTxt);
}

bool BodyDetector::detectBody(const Frame &frame)
{
    _modelSet.inputBlob = blobFromImage(frame.images[0], _modelSet.scale, cv::Size(_modelSet.W_in, _modelSet.H_in), cv::Scalar(0, 0, 0), false, false);
    _modelSet.net.setInput(_modelSet.inputBlob);
    _modelSet.result = _modelSet.net.forward();
    // the result is an array of "heatmaps", the probability of a body part being in location x,y
    int H = _modelSet.result.size[2];
    int W = _modelSet.result.size[3];
    // find the position of the body parts
    for (int n = 0; n < _modelSet.nparts; n++)
    {
        // Slice heatmap of corresponding body's part.
        cv::Mat heatMap(H, W, CV_32F, _modelSet.result.ptr(0,n));
        // 1 maximum per heatmap
        cv::Point pm;
        double conf;
        minMaxLoc(heatMap, nullptr, &conf, nullptr, &pm);
        if (conf > _modelSet.thresh) {

            continue;
        }
        return false;
    }
    return true;
}
