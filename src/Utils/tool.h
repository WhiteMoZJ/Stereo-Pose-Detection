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

    /**
     * @brief  A display method to show stereo camera frames in a frame
     * @param   frame Frame object
     */
    static void displayCameraFrame(Frame &frame);

    /**
     * @brief   With some info showing
     * @param   frame Frame object
     */
    static void displayCameraFrameInfo(Frame &frame);
};


#endif //TOOL_H
