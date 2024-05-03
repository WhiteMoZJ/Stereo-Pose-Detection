//
// Created by jun on 24-5-1.
//

#ifndef POINTSET_BUFFER_H
#define POINTSET_BUFFER_H

#include "../stdafx.h"
#include <Eigen/Dense>

struct PointSet
{
    size_t seq = 0;
    std::array<std::array<Eigen::Vector2i, 14>, 2> points;
};

class PointsetBuffer
{
public:
    explicit PointsetBuffer(size_t size);
    PointsetBuffer() = delete;
    PointsetBuffer(PointsetBuffer&) = delete;
    ~PointsetBuffer() = default;

    /**
     * @brief   Push PointSet object to frame buffer
     * @param   pointset PointSet object
     * @return  Is successful
     */
    bool push(const PointSet &pointset);

    /**
     * @brief   Get latest PointSet object from frame buffer
     * @param   pointset PointSet object to assign
     * @return  Is successful
     */
    bool getLatest(PointSet &pointset);

    /**
     * @brief   Get buffer empty status
     * @return  Empty status
     */
    bool isEmpty() const
    {
        return _pointsets.empty();
    }

private:
    std::vector<PointSet> _pointsets;
    std::vector<std::timed_mutex> _mutexs;

    size_t _tailIdx;
    size_t _headIdx;

    size_t _lastSeq;
};



#endif //POINTSET_BUFFER_H
