//
// Created by junchau on 10/22/23.
//

#include "thread_task.h"
#include <opencv2/core/utils/logger.hpp>

ThreadTask::ThreadTask() :
        _cameraPtr(std::make_unique<device::Camera>()),
        _detectorPtr(std::make_unique<BodyDetector>()),
        _frameBuffer(6),
        _pointsBuffer(6),
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
    int time = static_cast<int>(getTimeStamp());
    printf("[ INFO:END@%2d:%2d:%2d:%3d] Stream End\n",
           time/3600000, time/60000%60, time/1000%60, time % 1000);
    _cameraPtr->endStream();

    time = static_cast<int>(getTimeStamp());
    printf("[ INFO:END@%2d:%2d:%2d:%3d] Program Exit(0)\n",
           time/3600000, time/60000%60, time/1000%60, time % 1000);
}

void ThreadTask::init()
{
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);  // Silent OpenCV info
    _cameraPtr->setVideoFormat(640, 480);
    _cameraPtr->setUpStream();
    startTime = std::chrono::high_resolution_clock::now();
}

void ThreadTask::produce()
{
    int time = static_cast<int>(getTimeStamp());
    printf("[ INFO:START@%2d:%2d:%2d:%3d] Start Streaming (Press ESC to exit)\n",
           time/3600000, time/60000%60, time/1000%60, time % 1000);
//    auto t1 = std::chrono::high_resolution_clock::now();
    while (cv::waitKey(1) != 27) {  // press esc to exit
        if (!_consume_signal) break;    // detect if consume thread is terminated

        std::unique_lock<std::mutex> lock_frame(_mFrame);

        _cameraPtr->startStream();
        std::array<cv::Mat, 2> images;

        *_cameraPtr >> images;
        _produce_signal = _frameBuffer.push(Frame{{images[0], images[1]}, _cameraPtr->getFrameCount(), getTimeStamp()});

        // std::cout << timeStamp << std::endl;

        // Capture period: about 11 ms (1000ms/90)
//        auto t2 = std::chrono::high_resolution_clock::now();
//        std::cout << "Capture period: " << (static_cast<std::chrono::duration<double, std::milli>>(t2 - t1)).count() << " ms" << std::endl;
//        t1 = t2;

    }
    _produce_signal = false;
}

void ThreadTask::consume()
{
    // let this thread sleep for 2ms to prevent read empty buffer
    std::this_thread::sleep_for(std::chrono::duration<double,std::milli>(2));
    while (cv::waitKey(1) != 27) {

        std::unique_lock<std::mutex> lock_frame(_mFrame);
        std::unique_lock<std::mutex> lock_points(_mPoint);
        Frame frame;
        PointSet point_set;
        if (!_produce_signal) break;    // detect if produce thread is terminated
        if (!_frameBuffer.getLatest(frame)) {
            int time = static_cast<int>(getTimeStamp());
            printf("[ INFO:WARNING@%2d:%2d:%2d:%3d] buffer warning:frame lost\n",
                   time/3600000, time/60000%60, time/1000%60, time % 1000);
            continue;
        }
        lock_frame.unlock();
        // !ERROR
//        if (!frame.empty())
//            _detectorPtr->detectBody(frame, point_set);
        Tool::displayCameraFrame(frame);
    }
    _consume_signal = false;
}



