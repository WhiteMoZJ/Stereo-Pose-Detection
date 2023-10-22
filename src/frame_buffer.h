//
// Created by junchau on 10/22/23.
//

#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include <mutex>
#include <chrono>
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

struct Frame
{
    cv::Mat img[2];
    size_t seq;
    double timeStamp;
};

//class FrameBuffer
//{
//public:
//    FrameBuffer(size_t size);
//
//    ~FrameBuffer() = default;
//
//    bool push(const Frame& frame);
//
//    bool getLatest(Frame& frame);
//
//private:
//    std::vector<Frame> _frames;
//    std::vector<std::timed_mutex> _mutexs;
//
//    size_t _tailIdx;
//    size_t _headIdx;
//
//    double _lastGetTimeStamp;
//};


#endif //FRAME_BUFFER_H
