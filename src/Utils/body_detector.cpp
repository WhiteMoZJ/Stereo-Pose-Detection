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
        for (auto &j : i)
            j = std::make_unique<KalmanFilter>(
                Eigen::Vector3f(_cameraSettings.intrinsics(0, 2), _cameraSettings.intrinsics(1, 2), 0), 100, 3);

    for (auto & i : _kalman_filter_z)
        i = std::make_unique<KalmanFilter>(Eigen::Matrix<float, 4, 1>::Ones(), 0.5, 10);

    for (auto &i : _current_point_array)
        i = Eigen::Matrix<float, 15, 2>::Zero();

}

BodyDetector::~BodyDetector()
{
#ifdef DEBUG
    outfile_filter.close();
    outfile_raw.close();
#endif
}

SpacePoints BodyDetector::detectBody(const Frame &frame)
{
    // Check if the frame is empty
    if (frame.isEmpty()) return SpacePoints{};
    PointArray points{};

    bool flag = false;

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
            }
            _kalman_filter[i][n]->predict(Eigen::Vector3f(points[i](n, 0), points[i](n, 1), 1));
#ifdef DEBUG
            if (i == 0 && n == 1) {
                outfile_raw << points[i](n, 0) << "," << points[i](n, 1) << " 0 0\n";
            }
#endif

            if (!flag) {
                _frameCount ++;
                points[i](n, 0) = _kalman_filter[i][n]->getState()(0);
                points[i](n, 1) = _kalman_filter[i][n]->getState()(1);
                _current_point_array[i] = points[i];


            }
#ifdef DEBUG
            if (i == 0)
                // std::cout << points[i] << std::endl;

            if (i == 0 && n == 1) {
                outfile_filter << points[i](n, 0) << "," << points[i](n, 1) << " 0 0\n";
            }
#endif
        }
    }

#ifdef DEBUG

    // create 2 black gray images
    cv::Mat outimg_l = frame.images[0].clone();
    cv::cvtColor(outimg_l, outimg_l, cv::COLOR_GRAY2BGR);

    // cv::Mat outimg_r = frame.images[1].clone();
    // cv::cvtColor(outimg_r, outimg_r, cv::COLOR_GRAY2BGR);

    for (int j = 0; j < 15; j++) {
        cv::circle(outimg_l, cv::Point(_current_point_array[0](j, 0), _current_point_array[0](j, 1)), 5, cv::Scalar(0, 255, 0), -1);
        cv::putText(outimg_l, std::to_string(j), cv::Point(_current_point_array[0](j, 0), _current_point_array[0](j, 1) - 10), cv::FONT_HERSHEY_SIMPLEX , 0.5, cv::Scalar(0, 255, 0), 2);
    //     cv::circle(outimg_r, cv::Point(_current_point_array[1](j, 0), _current_point_array[1](j, 1)), 5, cv::Scalar(0, 255, 0), -1);
    //     cv::putText(outimg_r, std::to_string(j), cv::Point(_current_point_array[1](j, 0), _current_point_array[1](j, 1) - 10), cv::FONT_HERSHEY_SIMPLEX , 0.5, cv::Scalar(0, 255, 0), 2);
    }
    cv::imshow("outimg_l", outimg_l);
    // cv::imshow("outimg_r", outimg_l);
    cv::waitKey(1);
#endif

    solve3D(points);
    return _spacePoints;
}


bool BodyDetector::solve3D(const PointArray& points)
{
    static Eigen::Matrix<float, 15, 1> cur_z = Eigen::Matrix<float, 15, 1>::Ones();
    Eigen::Matrix<float, 15, 1> z = Eigen::Matrix<float, 15, 1>::Ones();
    float x[15], y[15];

    Eigen::Matrix<float, 15, 2> points_l = points[0], points_r = points[1];
    for (int i = 0; i < 15; i++) {
        if (points_l(i,0) == points_r(i,0))
            z(i) = cur_z(i);
        else {
            z(i) = _cameraSettings.baseline * _cameraSettings.intrinsics(0, 0) / (points_l(i,0) - points_r(i,0) + 500);
            cur_z(i) = z(i);
        }

        _kalman_filter_z[i]->predict(Eigen::Vector4f(x[i], y[i], z(i), 1));
        // z(i) = _kalman_filter_z[i]->getState().z();

        x[i] = (points_l(i,0) - _cameraSettings.intrinsics(0, 2)) * z(i) /  _cameraSettings.intrinsics(0, 0);
        y[i] = (- points_l(i,1) - _cameraSettings.intrinsics(1, 2)) * z(i) /  _cameraSettings.intrinsics(1, 1);

        // _spacePoints[i] = _kalman_filter_z[i]->getState() / 10 ;
        _spacePoints[i] = Eigen::Vector4f(x[i], y[i], z(i), 1);

        std::cout << z(i) << "\n";
    }


#ifdef DEBUG
    // std::cout << _spacePoints[1].transpose() << "\n";
#endif
    return true;
}
