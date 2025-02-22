//
// Created by junchau on 10/23/23.
//

#include "body_detector.h"
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/highgui.hpp>

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
        i = std::make_unique<KalmanFilter>(
            Eigen::Vector3f(_cameraSettings.intrinsics(0, 0), _cameraSettings.intrinsics(1, 1), 0), 200, 5);

    _kalman_filter_z = std::make_unique<KalmanFilter>(Eigen::Matrix<float, 15, 1>::Identity(), 100, 10);

}

SpacePoints BodyDetector::detectBody(const Frame &frame)
{
    // Check if the frame is empty
    if (frame.isEmpty()) return SpacePoints{};
    PointArray points{};

    bool flag = false;

    // create 2 black gray images
    cv::Mat outimg_l = frame.images[0].clone();
    cv::cvtColor(outimg_l, outimg_l, cv::COLOR_GRAY2BGR);

    // Loop over the images in the frame
    for (int i = 0; i < 2; i++) {
        cv::Mat dst;
        // Create an array of images
        cv::cvtColor(frame.images[i], dst, cv::COLOR_GRAY2BGR);

        // Create a blob from the image
        cv::Mat inputBlob = cv::dnn::blobFromImage(dst, scale,
            cv::Size(_cameraSettings.getResolution().width, _cameraSettings.getResolution().height), cv::Scalar(0, 0, 0), false, false);
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
                p.x *= _cameraSettings.getResolution().width / W;
                p.y *= _cameraSettings.getResolution().height / H;

                points[i](n, 0) = static_cast<float>(p.x);
                points[i](n, 1) = static_cast<float>(p.y);
            }
            else {
                points = _current_point_array;
                flag = true;
                break;
            }
        }

        if (!flag) {
            _frameCount ++;
            _kalman_filter[i]->update(Eigen::Vector3f(points[i](0, 0), points[i](0, 1), _frameCount));
            _kalman_filter[i]->predict();

            points[i](0, 0) = _kalman_filter[i]->getState()(0);
            points[i](0, 1) = _kalman_filter[i]->getState()(1);

            _current_point_array[i] = points[i];
        }
    }

#ifdef DEBUG
    for (int j = 0; j < 15; j++) {
        cv::circle(outimg_l, cv::Point(points[0](j, 0), points[0](j, 1)), 5, cv::Scalar(0, 255, 0), -1);
        cv::putText(outimg_l, std::to_string(j), cv::Point(points[0](j, 0), points[0](j, 1) - 10), cv::FONT_HERSHEY_SIMPLEX , 0.5, cv::Scalar(0, 255, 0), 2);
    }
#endif


    cv::imshow("outimg_l", outimg_l);
    // cv::imshow("outimg_r", frame.images[0]);
    cv::waitKey(1);
    //

    solve3D(points);
    return _spacePoints;
}


bool BodyDetector::solve3D(const PointArray& points)
{

    Eigen::Matrix<float, 15, 1> z = Eigen::Matrix<float, 15, 1>::Identity();
    float x, y;

    Eigen::Matrix<float, 15, 2> points_l = points[0], points_r = points[1];
    for (int i = 0; i < 15; i++) {
        if (points_l(i, 0) == points_r(i, 0)) {
            _kalman_filter_z->update(Eigen::Matrix<float, 15, 1>::Identity());
            return false;
        }
        x = (points_l(i,0) + points_r(i,0)) / 2.0f
                - _cameraSettings.intrinsics(0, 2);
        y = -(points_l(i,1) + points_r(i,1)) / 2.0f
                - _cameraSettings.intrinsics(1, 2);

        z(i) = _cameraSettings.baseline * _cameraSettings.intrinsics(0, 0) / (points_l(i,0) - points_r(i,0));
    }
    _kalman_filter_z->update(z);
    _kalman_filter_z->predict();
    z = _kalman_filter_z->getState();
    for (int i = 0; i < 15; i++) {
        _spacePoints[i] = Eigen::Vector3f(x, y, z(i));
    }
#ifdef DEBUG
    std::cout << "[x: " << points_l(0,0) << " y: " << points_l(0,1) << " z: " << z(0) << "]\n";
#endif
    return true;
}
