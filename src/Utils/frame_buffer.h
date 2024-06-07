//
// Created by junchau on 10/22/23.
// Reference: https://github.com/SEU-SuperNova-CVRA/Robomaster2018-SEU-OpenSource
//

#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include "../stdafx.h"

/**
 * @brief   Represents a frame with two images and a timestamp.
 */
struct Frame
{
    std::array<cv::Mat, 2> images{}; // Array of two images
    size_t seq = 0; // Sequence number
    double timeStamp = 0.f; // Timestamp

    /**
     * @brief   Checks if both stereo frames are empty.
     * @return  True if both frames are empty, false otherwise.
     */
    bool isEmpty() const
    {
        return (images[0].empty() || images[1].empty());
    }
};

/**
 * @brief   Represents a frame buffer that stores Frame objects.
 */
class FrameBuffer
{
public:
    /**
     * @brief   Constructs a FrameBuffer object with the specified size.
     * @param   size The size of the frame buffer.
     */
    explicit FrameBuffer(size_t size);

    FrameBuffer() = delete;
    FrameBuffer(FrameBuffer&) = delete;

    ~FrameBuffer() = default;

    /**
     * @brief   Pushes a Frame object to the frame buffer.
     * @param   frame The Frame object to push.
     * @return  True if the push operation is successful, false otherwise.
     */
    bool push(const Frame &frame);

    /**
     * @brief   Gets the latest Frame object from the frame buffer.
     * @param   frame The Frame object to assign the latest frame to.
     * @return  True if the get operation is successful, false otherwise.
     */
    bool getLatest(Frame &frame);

    /**
     * @brief   Swaps the latest frame from this buffer to the specified buffer.
     * @param   buffer The FrameBuffer object to swap the latest frame to.
     * @return  True if the swap operation is successful, false otherwise.
     */
    bool swapLatestTo(FrameBuffer &buffer) const;

    /**
     * @brief   Checks if the frame buffer is empty.
     * @return  True if the frame buffer is empty, false otherwise.
     */
    bool isEmpty() const
    {
        return _frames.empty();
    }

private:
    std::vector<Frame> _frames;             // Vector of Frame objects
    std::vector<std::timed_mutex> _mutexs;  // Vector of timed mutexes

    size_t _tailIdx;                        // Index of the tail frame
    size_t _headIdx;                        // Index of the head frame

    double _lastGetTimeStamp;               // Timestamp of the last get operation
};

#endif //FRAME_BUFFER_H
