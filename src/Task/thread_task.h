//
// Created by junchau on 10/22/23.
//

#ifndef THREAD_TASK_H
#define THREAD_TASK_H

#include "../Device/camera.h"
#include "utils.h"
#include "../stdafx.h"

/**
 * @class ThreadTask
 * @brief Represents a thread task for camera frame processing and display.
 */
class ThreadTask
{
public:
    ThreadTask();
    ThreadTask(const ThreadTask&) = delete;
    ~ThreadTask();

    /**
     * @brief Initializes the thread task.
     */
    void init();

    /**
     * @brief Loads camera frame to buffer.
     */
    void produce();

    /**
     * @brief Reads frame from buffer, detects body, and calculates 3D points.
     */
    void consume();

    /**
     * @brief Displays the processed frame in the GUI.
     */
    void display();

private:
    std::unique_ptr<Camera> _cameraPtr;             // unique Camera object
    std::unique_ptr<BodyDetector> _detectorPtr;     // unique BodyDetector object
    std::unique_ptr<Gui> _guiPtr;

    bool _isShoutdown; // controlled by display thread
    bool _canDisplay;

    std::mutex _mtx;
    FrameBuffer _middleBuffer, _backBuffer;
    timer _startTime;

    /**
     * @brief Outputs thread information in the command line.
     * @param type The type of information (START, END, WARNING, ERROR, etc.).
     * @param info The information to display.
     */
    inline void threadInfo(MSGType type, const char *info) const;

    /**
     * @brief Gets the current time in milliseconds.
     * @return The time stamp in milliseconds.
     */
    double getTimeStamp() const
    {
        return (static_cast<std::chrono::duration<double, std::milli>>(getNow() - _startTime)).count();
    }
};

#endif //THREAD_TASK_H
