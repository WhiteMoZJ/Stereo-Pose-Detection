//
// Created by junchau on 10/22/23.
//

#ifndef THREAD_TASK_H
#define THREAD_TASK_H

/*
 * Macro to control info output
 * It will be changed to an option
 */
#define INFO
// #define TIMER

#include "frame_buffer.h"
#include "points_buffer.h"
#include "../Device/camera.h"
#include "utils.h"
#include "../stdafx.h"

typedef std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<double,std::milli>> timer;

enum MSGType
{
    MSG_START   = 0,
    MSG_END     = 1,
    MSG_EXIT    = 2,
    MSG_WARN    = 3,
    MSG_ERR     = 4
};

class ThreadTask
{
public:
    ThreadTask();
    ThreadTask(const ThreadTask&) = delete; // Non-copied thread
    ~ThreadTask();

    /**
     * @brief   Initiate thread setting
     */
    void init();

    /**
     * @brief   Load camera frame to buffer
     */
    void produce();

    /**
     * @brief   Read frame from buffer

     Detect body & Calculate 3D points
     */
    void consume();

    void display();

private:
    std::unique_ptr<device::Camera> _cameraPtr;             // unique Camera object
    // std::unique_ptr<BodyDetector> _detectorPtr;             // unique BodyDetector object
    std::unique_ptr<Gui> _guiPtr;

    FrameBuffer _frontBuffer, _backBuffer;
    PointsBuffer _pointsBuffer;
    bool _signal, _dis;
    // _signal to detect thread status
    // _dis to control v-sync
    std::condition_variable _cv;

    Frame _displayFrame;

    timer startTime;

    std::array<const char*, 5> _msgs{"START", "END", "EXIT", "WARNING", "ERROR"};

    /**
     * @brief  Output thread information in cmd.
     * @param type Info type(START END WARNING ERROR...)
     * @param info Information
    */
    inline void threadInfo(MSGType type, const char *info);

    /**
     * @brief   Get current time in ms
     * @return  Time stamp
     */
    double getTimeStamp()
    {
        return (static_cast<std::chrono::duration<double,std::milli>>(std::chrono::high_resolution_clock::now() - startTime)).count();
    }

#ifdef TIMER
    timer start;
#endif
};


#endif //THREAD_TASK_H
