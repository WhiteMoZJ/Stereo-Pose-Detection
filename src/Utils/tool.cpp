//
// Created by junchau on 10/23/23.
//

#include "tool.h"

void debug::Tool::displayCameraFrame(Frame &frame)
{
    cv::Mat merged_img(frame.images[0].rows, frame.images[0].cols * 2 + 1, frame.images[0].type(), cv::Scalar(0));
    frame.images[0].copyTo(merged_img.colRange(0, frame.images[0].cols));
    frame.images[1].copyTo(merged_img.colRange(frame.images[1].cols + 1, merged_img.cols));

    cv::imshow("camera", merged_img);
}
