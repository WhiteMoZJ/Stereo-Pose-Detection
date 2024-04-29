//
// Created by junchau on 10/23/23.
//

#include "body_detector.h"

BodyDetector::BodyDetector()
{
    net = cv::dnn::readNet(modelBin, modelTxt);
}

bool BodyDetector::detectBody(Frame &frame)
{
    if (frame.isEmpty()) return false;

    for (int i = 0; i < 2; i++) {
        cv::Mat inputBlob = cv::dnn::blobFromImage(frame.images[i], scale,
            cv::Size(W_in, H_in), cv::Scalar(0, 0, 0), false, false);
        net.setInput(inputBlob);
        cv::Mat result = net.forward();

        const int H = result.size[2], W = result.size[3];

        // find the position of the body parts;
        for (int n=0; n < nparts; n++)
        {
            // Slice heatmap of corresponding body's part.
            cv::Mat heatMap(H, W, CV_32F, result.ptr(0,n));
            // 1 maximum per heatmap
            cv::Point p(-1,-1),pm;
            double conf;
            minMaxLoc(heatMap, 0, &conf, 0, &pm);
            if (conf > thresh)
                p = pm;
            // frame.points[i][n] = p;
        }
    }
    std::cout << "yes!!!!\n";

    return true;
}
