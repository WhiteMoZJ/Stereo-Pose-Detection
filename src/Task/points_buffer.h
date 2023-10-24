//
// Created by junchau on 10/24/23.
//

#ifndef POINTS_BUFFER_H
#define POINTS_BUFFER_H

#include "../stdafx.h"

struct PointSet
{
    // Each point set contains:
    std::array<cv::Point, 22> points;         // solved points in 3D
    size_t frameSeq;    // from which frame
    size_t pointSeq;    // its own seq
    double timeStamp;
};

// This buffer to load solved 3D point set

class PointsBuffer
{
public:
    explicit PointsBuffer(size_t size);

    ~PointsBuffer() = default;

    bool push(const PointSet& points);

    bool getLatest(PointSet& points);

private:
    std::vector<PointSet> _point_sets;
    std::vector<std::timed_mutex> _mutexs;

    size_t _tailIdx;
    size_t _headIdx;

    double _lastGetTimeStamp;
};


#endif //POINTS_BUFFER_H
