//
// Created by junchau on 10/22/23.
//

#include "frame_buffer.h"

FrameBuffer::FrameBuffer(size_t size):
        _frames(size),
        _mutexs(size),
        _tailIdx(0),
        _headIdx(0),
        _lastGetTimeStamp(0.0)
{ }

bool FrameBuffer::push(const Frame& frame)
{
    const size_t newHeadIdx = (_headIdx + 1) % _frames.size();

    //try for 1ms to lock
    std::unique_lock<std::timed_mutex> lock(_mutexs[newHeadIdx],std::chrono::milliseconds(1));
    if(!lock.owns_lock())
    {
        return false;
    }

    _frames[newHeadIdx] = frame;
    if(newHeadIdx == _tailIdx)
    {
        _tailIdx = (_tailIdx + 1) % _frames.size();
    }
    _headIdx = newHeadIdx;
    return true;
}

bool FrameBuffer::getLatest(Frame& frame)
{
    volatile const size_t headIdx = _headIdx;

    //try for 1ms to lock
    std::unique_lock<std::timed_mutex> lock(_mutexs[headIdx], std::chrono::milliseconds(1));
    if (!lock.owns_lock() ||
        (_frames[headIdx].images[0].empty() && _frames[headIdx].images[1].empty()) ||
        _frames[headIdx].timeStamp == _lastGetTimeStamp) {
        return false;
    }

    frame = _frames[headIdx];
    _lastGetTimeStamp = _frames[headIdx].timeStamp;

    return true;
}