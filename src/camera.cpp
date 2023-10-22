//
// Created by junchau on 10/18/23.
//

#include "camera.h"

Camera::Camera() : width(640), height(480)
{
    cfg.enable_stream(RS2_STREAM_INFRARED, 1, width, height, RS2_FORMAT_Y8);
    cfg.enable_stream(RS2_STREAM_INFRARED, 2, width, height, RS2_FORMAT_Y8);
}

void Camera::startStream()
{
    // disable emitter
    rs2::pipeline_profile selection = pipe.start(cfg);
    rs2::device selected_device = selection.get_device();
    auto depth_sensor = selected_device.first<rs2::depth_sensor>();
    if (depth_sensor.supports(RS2_OPTION_EMITTER_ENABLED))
        depth_sensor.set_option(RS2_OPTION_EMITTER_ENABLED, 0.f);

    while (cv::waitKey(1) < 0) {
        rs2::frameset frameset = pipe.wait_for_frames();

        // get left and right infrared frames from frameset
        rs2::video_frame ir_frame_left = frameset.get_infrared_frame(1);
        rs2::video_frame ir_frame_right = frameset.get_infrared_frame(2);

        frame_left.dMat = cv::Mat(cv::Size(width, height), CV_8UC1, (void*)ir_frame_left.get_data());
        frame_right.dMat = cv::Mat(cv::Size(width, height), CV_8UC1, (void*)ir_frame_right.get_data());

        cv::imshow("Left", frame_left.dMat);
        cv::imshow("Right", frame_right.dMat);
    }
}

void Camera::endStream()
{
    pipe.stop();
}
