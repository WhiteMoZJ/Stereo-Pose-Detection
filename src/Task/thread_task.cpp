//
// Created by junchau on 10/22/23.
//

#include "thread_task.h"

ThreadTask::ThreadTask() :
    _cameraPtr(std::make_unique<device::Camera>()),
    _buffer(6),
    _produce_signal(false),
    _consume_signal(true)
    // set consume signal true to make consume working first to start up produce thread
    // 1. produce thread terminated --> _produce_signal false
    // the consume thread will be terminated, _consume_signal false
    // 2. consume thread terminated --> _consume_signal false
    // then produce thread will be terminated, _produce_signal false
{

}

ThreadTask::~ThreadTask()
{
    _cameraPtr->endStream();
}

void ThreadTask::init()
{
    _cameraPtr->setVideoFormat(640, 480);
    _cameraPtr->setUpStream();
}

void ThreadTask::produce()
{
    auto startTime = std::chrono::high_resolution_clock::now();
//    auto t1 = std::chrono::high_resolution_clock::now();
    while (cv::waitKey(1) != 27) {  // press esc to exit
        std::unique_lock<std::mutex> lock(_M_frame);
        _cameraPtr->startStream();
        cv::Mat _frames[2];
        _frames[0] = _cameraPtr->left;
        _frames[1] = _cameraPtr->right;
        double timeStamp = (static_cast<std::chrono::duration<double,std::milli>>(std::chrono::high_resolution_clock::now() - startTime)).count();
        _produce_signal = _buffer.push(Frame{{_frames[0], _frames[1]}, _cameraPtr->getFrameCount(), timeStamp});

        if (!_consume_signal) break;    // detect if consume thread is terminated

        // Capture period: about 11 ms (1000ms/90)
//        auto t2 = std::chrono::high_resolution_clock::now();
//        std::cout << "Capture period: " << (static_cast<std::chrono::duration<double, std::milli>>(t2 - t1)).count() << " ms" << std::endl;
//        t1 = t2;

    }
    _produce_signal = false;
}

void ThreadTask::consume()
{
    while (cv::waitKey(1) != 27) {

        Frame frame;
        std::unique_lock<std::mutex> lock(_M_frame);

        if (!_produce_signal) break;    // detect if produce thread is terminated

        if (!_buffer.getLatest(frame)) {
            int time = static_cast<int>(frame.timeStamp);
            std::cout << "[" << time/3600000 << ":" << time/60000%60 << ":" << time/1000%60 << ":" << time % 1000 << "]: buffer warning" << std::endl;
            continue;
        }

        cv::imshow("left", frame.img[0]);
        cv::imshow("right", frame.img[1]);

    }
    _consume_signal = false;
    cv::destroyAllWindows();
}



