//
// Created by junchau on 10/23/23.
//

#include "body_detector.h"

BodyDetector::BodyDetector()
{
    net = cv::dnn::readNet(modelBin, modelTxt);
    net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
    net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);

#ifdef DEBUG
    if (net.empty())
    {
        std::cerr << "Can't load network by using the following files:"  << std::endl;
        std::cerr << "prototxt:   " << modelTxt << std::endl;
        std::cerr << "caffemodel: " << modelBin << std::endl;
    }else{
        std::cout << "pose.caffemodel was loaded successfully!" << std::endl;
    }
#endif
}

bool BodyDetector::detectBody(const Frame &frame)
{
    if (frame.isEmpty()) return false;

    try {
        const cv::Mat imgv[2][3] = {{frame.images[0], frame.images[0], frame.images[0]},
            {frame.images[1], frame.images[1], frame.images[1]}};

        PointSet pointset;
        pointset.seq = frame.seq;
        for (int i = 0; i < 2; i++) {
            cv::Mat dst;

            merge(imgv[i], 3, dst);

            cv::Mat inputBlob = cv::dnn::blobFromImage(dst, scale,
                cv::Size(368,368), cv::Scalar(0, 0, 0), true, false);
            net.setInput(inputBlob);
            cv::Mat prob_4D = net.forward();

            const int H = prob_4D.size[2], W = prob_4D.size[3];

            // find the position of the body parts;
            for (int n = 0; n < nparts; n++)
            {
                // Slice heatmap of corresponding body's part.
                cv::Mat heatMap(H, W, CV_32F, prob_4D.ptr(0, n));

                // 1 maximum per heatmap
                cv::Point p(-1, -1),pm;
                double conf;
                minMaxLoc(heatMap, nullptr, &conf, nullptr, &pm);

                if (conf > thresh)
                    p = pm;
                pointset.points[i][n] = p;
            }
        }
#ifdef DEBUG
        std::cout << pointset.points[0][0] << "\n";
#endif
        // _pointsbuffer.push(pointset);
        return true;
    }
    catch (std::exception &e) {
        return false;
    }
}
