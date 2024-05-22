//
// Created by junchau on 10/22/23.
//

#ifndef THREAD_TASK_H
#define THREAD_TASK_H

#include "../Device/camera.h"
#include "utils.h"
#include "../stdafx.h"

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

    /**
     * @brief   Display in GUI
     */
    void display();

private:
    std::unique_ptr<Camera> _cameraPtr;             // unique Camera object
    std::unique_ptr<BodyDetector> _detectorPtr;             // unique BodyDetector object
    std::unique_ptr<Gui> _guiPtr;

    bool _isShoutdown; // controlled by display thread
    bool _canDisplay;

    std::mutex _mtx;
    FrameBuffer _middleBuffer, _backBuffer;
    timer _startTime;

    /**
     * @brief Output thread information in cmd.
     * @param type Info type(START END WARNING ERROR...)
     * @param info Information
    */
    inline void threadInfo(MSGType type, const char *info) const;

    /**
     * @brief   Get current time in ms
     * @return  Time stamp
     */
    double getTimeStamp() const
    {
        return (static_cast<std::chrono::duration<double,std::milli>>
         (getNow() - _startTime)).count();
    }
};


#endif //THREAD_TASK_H
