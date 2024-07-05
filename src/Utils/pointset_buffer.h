//
// Created by jun on 24-5-1.
//

#ifndef POINTSET_BUFFER_H
#define POINTSET_BUFFER_H

#include "../stdafx.h"

/**
 * @brief Represents a set of points with a sequence number.
 */
struct PointSet
{
    size_t seq = 0; /**< The sequence number of the point set. */
    SpacePoints points{}; /**< The array of points. */
};

/**
 * @brief A buffer for storing and retrieving PointSet objects.
 */
class PointsetBuffer
{
public:
    /**
     * @brief Constructs a PointsetBuffer object with the specified size.
     * @param size The size of the buffer.
     */
    explicit PointsetBuffer(size_t size);

    PointsetBuffer() = delete;
    PointsetBuffer(PointsetBuffer&) = delete;
    ~PointsetBuffer() = default;

    /**
     * @brief Pushes a PointSet object to the frame buffer.
     * @param pointset The PointSet object to push.
     * @return True if the push operation is successful, false otherwise.
     */
    bool push(const PointSet &pointset);

    /**
     * @brief Gets the latest PointSet object from the frame buffer.
     * @param pointset The PointSet object to assign the latest PointSet to.
     * @return True if the get operation is successful, false otherwise.
     */
    bool getLatest(PointSet &pointset);

    /**
     * @brief Checks if the buffer is empty.
     * @return True if the buffer is empty, false otherwise.
     */
    bool isEmpty() const
    {
        return _pointsets.empty();
    }

private:
    std::vector<PointSet> _pointsets;       // The vector of PointSet objects.
    std::vector<std::timed_mutex> _mutexs;  // The vector of timed mutexes.

    size_t _tailIdx;                        // The index of the tail element in the buffer.
    size_t _headIdx;                        // The index of the head element in the buffer.

    size_t _lastSeq;                        // The sequence number of the last PointSet object.
};

#endif //POINTSET_BUFFER_H
