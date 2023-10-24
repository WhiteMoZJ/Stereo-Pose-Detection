//
// Created by junchau on 10/24/23.
//

#include "points_buffer.h"

PointsBuffer::PointsBuffer(size_t size) :
        _point_sets(size),
        _mutexs(size),
        _tailIdx(0),
        _headIdx(0),
        _lastGetTimeStamp(0.0)
{ }

bool PointsBuffer::push(const PointSet& points)
{
    const size_t newHeadIdx = (_headIdx + 1) % _point_sets.size();

    //try for 1ms to lock
    std::unique_lock<std::timed_mutex> lock(_mutexs[newHeadIdx],std::chrono::milliseconds(1));
    if(!lock.owns_lock())
    {
        return false;
    }

    _point_sets[newHeadIdx] = points;
    if(newHeadIdx == _tailIdx)
    {
        _tailIdx = (_tailIdx + 1) % _point_sets.size();
    }
    _headIdx = newHeadIdx;
    return true;
}

bool PointsBuffer::getLatest(PointSet &points)
{
    volatile const size_t headIdx = _headIdx;

    //try for 1ms to lock
    std::unique_lock<std::timed_mutex> lock(_mutexs[headIdx],std::chrono::milliseconds(1));
    if(!lock.owns_lock() ||
       _point_sets[headIdx].points.empty() ||
       _point_sets[headIdx].timeStamp == _lastGetTimeStamp)
    {
        return false;
    }

    points = _point_sets[headIdx];
    _lastGetTimeStamp = _point_sets[headIdx].timeStamp;

    return true;
}