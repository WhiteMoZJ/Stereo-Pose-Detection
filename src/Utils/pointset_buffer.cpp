//
// Created by jun on 24-5-1.
//

#include "pointset_buffer.h"

PointsetBuffer::PointsetBuffer(const size_t size):
        _pointsets(size),
        _mutexs(size),
        _tailIdx(0),
        _headIdx(0),
        _lastSeq(0)
{ }

bool PointsetBuffer::push(const PointSet& pointset)
{
    const size_t newHeadIdx = (_headIdx + 1) % _pointsets.size();

    //try for 1ms to lock
    const std::unique_lock<std::timed_mutex> lock(_mutexs[newHeadIdx],std::chrono::milliseconds(1));
    if(!lock.owns_lock())
    {
        return false;
    }

    _pointsets[newHeadIdx] = pointset;
    if(newHeadIdx == _tailIdx)
    {
        _tailIdx = (_tailIdx + 1) % _pointsets.size();
    }
    _headIdx = newHeadIdx;
    return true;
}

bool PointsetBuffer::getLatest(PointSet& pointset)
{
    volatile const size_t headIdx = _headIdx;

    //try for 1ms to lock
    const std::unique_lock<std::timed_mutex> lock(_mutexs[headIdx], std::chrono::milliseconds(1));
    if (!lock.owns_lock() || _pointsets[headIdx].seq == _lastSeq)
        return false;

    pointset = _pointsets[headIdx];
    _lastSeq = _pointsets[headIdx].seq;

    return true;
}


