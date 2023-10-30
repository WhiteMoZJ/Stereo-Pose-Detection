//
// Created by junchau on 10/22/23.
//

#include "thread_task.h"
#include <opencv2/core/utils/logger.hpp>

ThreadTask::ThreadTask() :
        _cameraPtr(std::make_unique<device::Camera>()),
        // _detectorPtr(std::make_unique<BodyDetector>()),
        _guiPtr(std::make_unique<Gui>()),
        _frontBuffer(6),
        _backBuffer(6),
        _pointsBuffer(6),
        _signal(true),
        _dis(false)
{
    startTime = std::chrono::high_resolution_clock::now();

}

ThreadTask::~ThreadTask()
{
    threadInfo(MSG_END, "Stream End");
    _cameraPtr->endStream();
    threadInfo(MSG_EXIT, "Program Exit(0)");
}

void ThreadTask::init()
{
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);  // Silent OpenCV info
    _cameraPtr->setVideoFormat(640, 480);
//    _cameraPtr->setExposureTime(10);
    if (_cameraPtr->setUpStream()) {
        _cameraPtr->printInfo();
        threadInfo(MSG_START, "Camera Initiated");
    }
}

void ThreadTask::produce()
{
    threadInfo(MSG_START, "Start Streaming...");
    for (;;) {
        if (!_signal) break;
#ifdef TIMER
        start = std::chrono::high_resolution_clock::now();
#endif
        _cameraPtr->startStream();
        std::array<cv::Mat, 2> images;

        *_cameraPtr >> images;
        if (!_backBuffer.push(Frame{images, _cameraPtr->getFrameCount(), getTimeStamp()}))
            continue;

        // push frame to _frontBuffer
        _backBuffer.swapTo(_frontBuffer);

    }
    _signal = false;
}

void ThreadTask::consume()
{
    for (;;) {
        if (!_signal) break;

        Frame frame;
        if (_frontBuffer.empty() || !_frontBuffer.getLatest(frame)) {
            // Sometimes this thread will read buffer twice before next pushing
            _dis = false;
            continue;
        }
        _displayFrame = frame;

        // !ERROR openpose needs RGB frame but there's grey only
//        if (!frame.empty())
//            _detectorPtr->detectBody(frame, point_set);

        _dis = true;
#ifdef TIMER
        // This is frame loading time
        timer end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double,std::milli> duration = end - start;
        std::cout << duration.count() << "ms" << std::endl;
        start = end;
#endif
    }
    _signal = false;
}

void ThreadTask::display()
{
    if (!_guiPtr->init("Pose Detection")) {
        threadInfo(MSG_ERR, "GUI Initiated Failed");
        _signal = false;
        return;
    }
    threadInfo(MSG_START, "GUI Initiated, Display Start");
    for(;;) {
        if (!_dis) continue;
        if (!_guiPtr->showImage(_displayFrame)) break;
    }

    _signal = false;
}

void ThreadTask::threadInfo(MSGType type, const char *info)
{
#ifdef INFO
    int time = static_cast<int>(getTimeStamp());
    if (type > _msgs.size() - 1) {
        printf("[ INFO@%02d:%02d:%02d:%03d] Message type undefined\n",
               time/3600000, time/60000%60, time/1000%60, time%1000);
    }
    printf("[ INFO@%02d:%02d:%02d.%03d] %s:%s\n",
           time/3600000, time/60000%60, time/1000%60, time%1000, _msgs[type], info);
#endif  //INFO
}



