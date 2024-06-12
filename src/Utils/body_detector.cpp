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
    // Check if the frame is empty
    if (frame.isEmpty()) return false;
    PointArray points{};

    // Loop over the images in the frame
    for (int i = 0; i < 2; i++) {
        cv::Mat dst;
        // Create an array of images
        const cv::Mat imgv[3]{frame.images[i], frame.images[i], frame.images[i]};
        // Merge the images into one
        cv::merge(imgv, 3, dst);

        // Create a blob from the image
        cv::Mat inputBlob = cv::dnn::blobFromImage(dst, scale,
            cv::Size(240,240), cv::Scalar(0, 0, 0), true, false);
        // Set the input to the network
        net.setInput(inputBlob);
        // Forward propagate through the network
        cv::Mat prob_4D = net.forward();

        const int H = prob_4D.size[2], W = prob_4D.size[3];

        // Loop over the body parts
        for (int n = 0; n < nparts; n++)
        {
            // Slice heatmap of corresponding body's part.
            cv::Mat heatMap(H, W, CV_32F, prob_4D.ptr(0, n));

            // Initialize a point to store the maximum of the heatmap
            cv::Point p(-1, -1), pm{};
            double conf;
            // Find the maximum of the heatmap
            minMaxLoc(heatMap, nullptr, &conf, nullptr, &pm);

            // If the maximum is greater than the threshold, update the point
            if (conf > thresh)
                p = pm;
            points[n][i].x() = p.x;
            points[n][i].y() = p.y;
        }
    }

    // Push the points to the buffer
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

    // for (int i = 0; i < 16; i++)
    // {
    //     // 双目深度估计
    //
    // }

    return SpacePoints{};
}
