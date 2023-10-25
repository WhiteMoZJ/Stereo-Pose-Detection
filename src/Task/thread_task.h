//
// Created by junchau on 10/22/23.
//

#ifndef THREAD_TASK_H
#define THREAD_TASK_H

#include "frame_buffer.h"
#include "points_buffer.h"
#include "../Device/camera.h"
#include "utils.h"
#include "../stdafx.h"

typedef std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<double,std::milli>> timer;

class ThreadTask
{
public:
    ThreadTask();
    ThreadTask(const ThreadTask&) = delete; // no copied thread
    ~ThreadTask();

    /*
     * @brief   Initiate thread setting
     */
    void init();

    /*
     * @brief   Load camera frame to buffer
     */
    void produce();

    /*
     * @brief   Read frame from buffer
     *          Detect body
     *          Calculate 3D points
     */
    void consume();

    /*
     * @brief   Display 3D body points in GUI
     */
    void display();


private:
    std::mutex _mFrame, _mPoint;
    std::unique_ptr<device::Camera> _cameraPtr;
    std::unique_ptr<BodyDetector> _detectorPtr;
    FrameBuffer _frameBuffer;
    PointsBuffer _pointsBuffer;
    bool _produce_signal, _consume_signal;

    timer startTime;

    /*
     * @brief   Output thread information in cmd
     * @param   *type Info type(START END WARNING ERROR...)
     * @param   *info Information
     */
    inline void threadInfo(const char *type, const char *info)
    {
        int time = static_cast<int>(getTimeStamp());
        printf("[ INFO:%s@%2d:%2d:%2d:%3d] %s\n",
               type, time/3600000, time/60000%60, time/1000%60, time % 1000, info);
    }

    /*
     * @brief   Get current time in ms
     * @return  Time stamp
     */
    double getTimeStamp()
    {
        return (static_cast<std::chrono::duration<double,std::milli>>(std::chrono::high_resolution_clock::now() - startTime)).count();
    }
};


#endif //THREAD_TASK_H
