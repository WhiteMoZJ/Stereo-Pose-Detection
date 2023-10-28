//
// Created by junchau on 10/22/23.
//

#include "thread_task.h"
#include <opencv2/core/utils/logger.hpp>

ThreadTask::ThreadTask() :
        _cameraPtr(std::make_unique<device::Camera>()),
        _detectorPtr(std::make_unique<BodyDetector>()),
        _frontBuffer(6),
        _backBuffer(6),
        _pointsBuffer(6),
        _produceSignal(true),
        _consumeSignal(true)
{
    startTime = std::chrono::high_resolution_clock::now();

}

ThreadTask::~ThreadTask()
{
    threadInfo("END", "Stream End");
    _cameraPtr->endStream();
    threadInfo("EXIT", "Program Exit(0)");
}

void ThreadTask::init()
{
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);  // Silent OpenCV info
    _cameraPtr->setVideoFormat(640, 480);
//    _cameraPtr->setExposureTime(10);
    if (_cameraPtr->setUpStream()) {
        _cameraPtr->printInfo();
        threadInfo("START", "Camera Initiated");
    }

}

void ThreadTask::produce()
{
    threadInfo("START", "Start Streaming...");
    for (;;) {
#ifdef TIMER
        start = std::chrono::high_resolution_clock::now();
#endif
        _cameraPtr->startStream();
        std::array<cv::Mat, 2> images;

        *_cameraPtr >> images;
        if (!_backBuffer.push(Frame{images, _cameraPtr->getFrameCount(), getTimeStamp()}))
            continue;

        // push  frame to _frontBuffer
        _backBuffer.swapTo(_frontBuffer);

        if (!_consumeSignal || cv::waitKey(1) == 27) break;
    }
    _produceSignal = false;
}

void ThreadTask::consume()
{
    threadInfo("START", "Start Displaying (Press ESC to exit)");
    for (;;) {
        Frame frame;

        if (_frontBuffer.empty() || !_frontBuffer.getLatest(frame)) {
            // Sometimes this thread will read buffer twice before next pushing
            continue;
        }
#ifdef TIMER
        // This is frame loading time
        timer end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double,std::milli> duration = end - start;
        std::cout << duration.count() << "ms" << std::endl;
        start = end;
#endif
        Tool::displayCameraFrame(frame);

//        std::unique_lock<std::mutex> lock_points(_mPoint);
//        PointSet point_set;

        // !ERROR openpose needs RGB frame but there's grey only
//        if (!frame.empty())
//            _detectorPtr->detectBody(frame, point_set);
        if (!_produceSignal || cv::waitKey(1) == 27) break;
    }
    _consumeSignal = false;
}

void ThreadTask::threadInfo(const char *type, const char *info)
{
#ifdef INFO
    int time = static_cast<int>(getTimeStamp());
    printf("[ INFO@%02d:%02d:%02d:%03d] %s:%s\n",
           time/3600000, time/60000%60, time/1000%60, time % 1000, type, info);
#endif  //INFO
}



