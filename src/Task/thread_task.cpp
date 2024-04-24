//
// Created by junchau on 10/22/23.
//

#include "thread_task.h"


ThreadTask::ThreadTask() :
        _cameraPtr(std::make_unique<device::Camera>()),
        _detectorPtr(std::make_unique<BodyDetector>()),
        _guiPtr(std::make_unique<Gui>()),
        _frontBuffer(6),
        _backBuffer(6),
        _signal(false),
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
    _cameraPtr->setVideoFormat(640, 480);
    _cameraPtr->setExposureTime(4.0816f);
    if (!_cameraPtr->setUpStream()) {
        _signal = false;
        threadInfo(MSG_ERR, "No device connect");
        return;
    }

    _cameraPtr->printInfo();
    threadInfo(MSG_START, "Camera Initiated");
    threadInfo(MSG_START, "Start Streaming...");
    _signal = true;
}

void ThreadTask::produce()
{
    for (;;) {
        if (!_signal) break;

        if (!_cameraPtr->startStream()) continue;
        std::array<cv::Mat, 2> images;
        *_cameraPtr >> images;

        if (!_backBuffer.push(Frame{images, _cameraPtr->getFrameCount(), getTimeStamp()}))
            continue;

        // push frame to _frontBuffer
        _backBuffer.swapTo(_frontBuffer);
    }
}

void ThreadTask::consume()
{
    for (;;) {
        if (!_signal) break;

        if (!_frontBuffer.getLatest(_displayFrame)) {
            // Sometimes this thread will read buffer twice before next pushing
            continue;
        }
        _dis = true;

        Frame frame = _displayFrame;



        // !WARNING openpose needs RGB frame
        // TODO: too slow with openpose (2fps)
//        if (!frame.empty()) {
//            _detectorPtr->detectBody(frame);
//        }


    }
}

void ThreadTask::display()
{
    // can not make init() & showImage() different threads
    if (!_guiPtr->init("Pose Detection")) {
        threadInfo(MSG_ERR, "GUI Initiated Failed");
        _signal = false;
    }
    threadInfo(MSG_START, "GUI Initiated, Waiting for stream...");

    for(;;) {
        if (!_dis) continue;
        if (!_guiPtr->update(_displayFrame, true)) break;
        // 1 ms lagging time to display
        _dis = false;
    }

    _signal = false;
}

void ThreadTask::threadInfo(MSGType type, const char *info) const
{
#ifdef DEBUG
    int time = static_cast<int>(getTimeStamp());
    if (type > _msgs.size() - 1) {
        printf("[ INFO@%02d:%02d:%02d:%03d] Message type undefined\n",
               time/3600000, time/60000%60, time/1000%60, time%1000);
    }
    printf("[ INFO@%02d:%02d:%02d.%03d] %s:%s\n",
           time/3600000, time/60000%60, time/1000%60, time%1000, _msgs[type], info);
#endif  //DEBUG
}



