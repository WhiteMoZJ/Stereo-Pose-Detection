//
// Created by junchau on 10/22/23.
//

#include "thread_task.h"

ThreadTask::ThreadTask() :
    _cameraPtr(std::make_unique<device::Camera>())
{

}

ThreadTask::~ThreadTask()
{
    _cameraPtr->endStream();
}

void ThreadTask::init()
{
    _cameraPtr->setUp();
    _cameraPtr->setVideoFormat(640, 480);
}

void ThreadTask::produce()
{
    Frame frame;
    while (cv::waitKey(1) < 0) {
        _M_frame.lock();
        _cameraPtr->startStream();
        frames[0] = _cameraPtr->left;
        frames[1] = _cameraPtr->right;
        _M_frame.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(12));
    }

}

void ThreadTask::consume()
{
    while (cv::waitKey(1) < 0) {
        _M_frame.lock();
        cv::imshow("left", frames[0]);
        cv::imshow("right", frames[1]);
        _M_frame.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(12));
    }
}



