//
// Created by junchau on 10/22/23.
//

#ifndef THREAD_TASK_H
#define THREAD_TASK_H

#include "frame_buffer.h"
#include "../Device/camera.h"
#include "utils.h"
#include "../stdafx.h"

class ThreadTask
{
public:
    ThreadTask();
    ThreadTask(const ThreadTask&) = delete; // no copied thread
    ~ThreadTask();

    // init camera
    void init();

    // load camera frame to buffer
    void produce();

    // run task;
    void consume();

private:
    std::mutex _mFrame;
    std::unique_ptr<device::Camera> _cameraPtr;
    FrameBuffer _buffer;
    bool _produce_signal, _consume_signal;
};


#endif //THREAD_TASK_H
