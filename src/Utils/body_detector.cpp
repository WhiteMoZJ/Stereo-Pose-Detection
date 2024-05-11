//
// Created by junchau on 10/23/23.
//

#include "body_detector.h"

float BodyDetector::_framerate;

BodyDetector::BodyDetector() : _pointsBuffer(3)
{
    _framerate = 0;
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
    PointArray points{};

    for (int i = 0; i < 2; i++) {
        cv::Mat dst;
        const cv::Mat imgv[3]{frame.images[i], frame.images[i], frame.images[i]};
        cv::merge(imgv, 3, dst);

        cv::Mat inputBlob = cv::dnn::blobFromImage(dst, scale,
            cv::Size(320,240), cv::Scalar(0, 0, 0), true, false);
        net.setInput(inputBlob);
        cv::Mat prob_4D = net.forward();

        const int H = prob_4D.size[2], W = prob_4D.size[3];

        // find the position of the body parts;
        for (int n = 0; n < nparts; n++)
        {
            // Slice heatmap of corresponding body's part.
            cv::Mat heatMap(H, W, CV_32F, prob_4D.ptr(0, n));

            // 1 maximum per heatmap
            cv::Point p(-1, -1), pm{};
            double conf;
            minMaxLoc(heatMap, nullptr, &conf, nullptr, &pm);

            if (conf > thresh)
                p = pm;
            points[i][n].x() = p.x;
            points[i][n].y() = p.y;
        }
    }

    _pointsBuffer.push(PointSet{frame.seq, points});
    return true;
}


SpacePoints BodyDetector::solve3D()
{
    PointSet set;
    _pointsBuffer.getLatest(set);
#ifdef DEBUG
    std::cout << set.points[0][0].transpose() << "\n";
#endif

    return SpacePoints{};
}
