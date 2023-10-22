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
    while (cv::waitKey(1) != 27) {  // press esc to exit
        _M_frame.lock();
        _cameraPtr->startStream();
        _frames[0] = _cameraPtr->left;
        _frames[1] = _cameraPtr->right;
        _M_frame.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(12));
    }
    _frames[0].release();
    _frames[1].release();
}

void ThreadTask::consume()
{
    while (cv::waitKey(1) != 27) {
        _M_frame.lock();
        if (_frames[0].empty() && _frames[1].empty()) {
            cv::destroyAllWindows();
            break;
        }
        cv::imshow("left", _frames[0]);
        cv::imshow("right", _frames[1]);
        _M_frame.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(12));
    }
}



