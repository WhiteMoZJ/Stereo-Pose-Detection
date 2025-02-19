//
// Created by junchau on 10/23/23.
//

#include "body_detector.h"
#include <opencv2/opencv.hpp>

BodyDetector::BodyDetector()
{
    _framerate = 0;
    net = cv::dnn::readNet(modelBin, modelTxt);
    if (cv::cuda::getCudaEnabledDeviceCount() > 0) {
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
    } else {
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
    }

    if (net.empty()) {
        throw std::runtime_error("Can't load network by using the following files:\nprototxt: " + modelTxt + "\ncaffemodel: " + modelBin);
    }

#ifdef DEBUG
    std::cout << "pose.caffemodel was loaded successfully!" << std::endl;
#endif

    // Initialize KalmanFilter in the constructor
    for (auto & i : _kalman_filter)
        i = std::make_unique<KalmanFilter>(Eigen::Vector3f(0, 0, 0));

}

SpacePoints BodyDetector::detectBody(const Frame &frame)
{
    // Check if the frame is empty
    if (frame.isEmpty()) return SpacePoints{};
    PointArray points{};

    bool flag = false;

    // create 2 black gray images
    cv::Mat outimg_l = cv::Mat::zeros(_cameraSettings.getResolution().height*2, _cameraSettings.getResolution().width*2, CV_8UC3);

    // Loop over the images in the frame
    for (int i = 0; i < 2; i++) {
        cv::Mat dst;
        // Create an array of images
        const cv::Mat imgv[3]{frame.images[i], frame.images[i], frame.images[i]};
        // Merge the images into one
        cv::merge(imgv, 3, dst);

        // Create a blob from the image
        cv::Mat inputBlob = cv::dnn::blobFromImage(dst, scale,
            cv::Size(_cameraSettings.getResolution().width/2, _cameraSettings.getResolution().height/2), cv::Scalar(0, 0, 0), false, false);
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
            if (conf > thresh) {
                p = pm;
                p.x *= _cameraSettings.getResolution().width * 2 / W;
                p.y *= _cameraSettings.getResolution().height * 2 / H;

                points[i](n, 0) = static_cast<float>(p.x);
                points[i](n, 1) = static_cast<float>(p.y);
            }
            else {
                points[i](n, 0) = -1;
                points[i](n, 1) = -1;
                flag = true;
            }
        }


        _kalman_filter[i]->predict();
        if (flag)
            continue;
        else {
            _kalman_filter[i]->update(Eigen::Vector3f(points[i](0, 0), points[i](0, 1), frame.timeStamp));
            points[i](0, 0) = _kalman_filter[i]->getState()(0);
            points[i](0, 1) = _kalman_filter[i]->getState()(1);
             {
                for (int j = 0; j < 15; j++) {
                    if (i == 0)
                        cv::circle(outimg_l, cv::Point(points[i](j, 0), points[i](j, 1)), 3, cv::Scalar(0, 255, 0), 1);
                    else cv::circle(outimg_l, cv::Point(points[i](j, 0), points[i](j, 1)), 3, cv::Scalar(255, 0, 0), 1);
                }
            }
        }
    }


    cv::imshow("outimg_l", outimg_l);
    // cv::imshow("outimg_r", frame.images[0]);
    cv::waitKey(1);
    //

    const SpacePoints space_points = solve3D(points);
    return space_points;
}


SpacePoints BodyDetector::solve3D(PointArray& points) const
{
    SpacePoints spacePoints{};

    for (int i = 0; i < 15; i++)
    {
        if (points[i](0, 0) == -1 || points[i](0, 0) == -1)
            spacePoints[i] = Eigen::Vector3f(-1, -1, -100);

        else {
            float x = ((points[i](0,0)) + (points[i](1,0))) / 2.0f
                        - (static_cast<float>(_cameraSettings.getResolution().width) / 2.0f);
            float y = -((points[i](0,1)) + (points[i](1,1))) / 2.0f
                        + (static_cast<float>(_cameraSettings.getResolution().height) / 2.0f);
            float z = 0.2f * _cameraSettings.baseline / (x - y);

            spacePoints[i] = Eigen::Vector3f(x, y, -z);
            std::cout << -z << "\n";
        }
    }
    return spacePoints;
}
