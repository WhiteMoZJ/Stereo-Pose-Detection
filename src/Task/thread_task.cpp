//
// Created by junchau on 10/22/23.
//

#include "thread_task.h"
#include <opencv2/core/utils/logger.hpp>

ThreadTask::ThreadTask() :
        _cameraPtr(std::make_unique<device::MVCamera>()),
        _detectorPtr(std::make_unique<BodyDetector>()),
        _guiPtr(std::make_unique<Gui>()),
        _frontBuffer(6),
        _backBuffer(6),
        _signal(true),
        _dis(true)
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
    _cameraPtr->setExposureTime(4.0816f);
    if (!_cameraPtr->setUpCam()) {
        _signal = false;
        threadInfo(MSG_ERR, "No device connect");
    }
    else {
//        _cameraPtr->printInfo();

        threadInfo(MSG_START, "Camera Initiated");
        threadInfo(MSG_START, "Start Streaming...");
    }
}

void ThreadTask::produce()
{
    for (;;) {
        if (!_signal) break;

        if (!_cameraPtr->startStream()) continue;

        std::array<cv::Mat, 2> images;

        *_cameraPtr >> images;

        _cameraPtr->endStream();

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
//        if (!frame.empty()) {
//            _detectorPtr->detectBody(frame);
//        }
        _dis = true;

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
    threadInfo(MSG_START, "GUI Initiated, Waiting for stream...");

    for(;;) {
        if (!_dis && _cameraPtr->isOpened()) continue;
        if (!_guiPtr->showImage(_displayFrame, _dis)) break;
        _dis = false;
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



