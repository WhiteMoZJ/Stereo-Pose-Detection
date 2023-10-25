//
// Created by junchau on 10/22/23.
// Reference: https://github.com/SEU-SuperNova-CVRA/Robomaster2018-SEU-OpenSource
//

#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include "../stdafx.h"

// frame: with 2 images and time stamp
struct Frame
{
    std::array<cv::Mat, 2> images;
    size_t seq = 0;
    double timeStamp = 0.f;
    bool empty() const
    {
        return images.empty();
    }
};

// frame buffer class
// to push the frames in it
class FrameBuffer
{
public:
    explicit FrameBuffer(size_t size);

    ~FrameBuffer() = default;

    /*
     * @brief   Push Frame object to frame buffer
     * @param   Frame object
     * @return  Is successful
     */
    bool push(const Frame& frame);

    /*
     * @brief   Get latest Frame object from frame buffer
     * @param   Frame object to assign
     * @return  Is successful
     */
    bool getLatest(Frame& frame);

private:
    std::vector<Frame> _frames;
    std::vector<std::timed_mutex> _mutexs;

    size_t _tailIdx;
    size_t _headIdx;

    double _lastGetTimeStamp;
};


#endif //FRAME_BUFFER_H
