//
// Created by junchau on 10/23/23.
//

#ifndef TOOL_H
#define TOOL_H

#include "../Task/frame_buffer.h"
#include "../stdafx.h"

class Tool
{
public:
    Tool() = default;
    static cv::Mat camFrame;
    static void displayCameraFrame(Frame &frame);
};


#endif //TOOL_H
