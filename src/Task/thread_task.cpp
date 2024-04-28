//
// Created by junchau on 10/22/23.
//

#include "thread_task.h"


ThreadTask::ThreadTask():
    _cameraPtr(std::make_unique<device::Camera>()),
    _detectorPtr(std::make_unique<BodyDetector>()),
    _guiPtr(std::make_unique<Gui>()),
    _isRunning(false),
    _canDisplay(false),
    _backBuffer(6)
{
    _startTime = std::chrono::high_resolution_clock::now();
}

ThreadTask::~ThreadTask()
{
    threadInfo(MSG_END, "Stream End");
    _cameraPtr->endStream();
    threadInfo(MSG_EXIT, "Program Exit(0)");
}

void ThreadTask::init()
{
    if (!_cameraPtr->setUpStream()) {
        _isRunning = false;
        threadInfo(MSG_ERR, "No device connect");
        return;
    }
    _isRunning = true;

    _cameraPtr->printInfo();
    threadInfo(MSG_START, "Camera Initiated");
    threadInfo(MSG_START, "Start Streaming...");
}

void ThreadTask::produce()
{
    for (;;) {
        if (!_isRunning) break;

        if (!_cameraPtr->startStream()) continue;
        std::array<cv::Mat, 2> images;
        *_cameraPtr >> images;

        if (!_backBuffer.push(Frame{images, _cameraPtr->getFrameCount(), getTimeStamp()}))
            continue;

        // push frame to _frontBuffer for display
        if (_backBuffer.swapTo(_guiPtr->frontBuffer))
            _canDisplay = true;
    }
}

void ThreadTask::consume()
{
    for (;;) {
        if (!_isRunning) break;
        // TODO: solve 3D pose
        Frame frame;
        if (!_backBuffer.getLatest(frame)) continue;
    }
}

void ThreadTask::display()
{
    if (!_guiPtr->init("Pose Detection")) {
        threadInfo(MSG_ERR, "GUI Initiated Failed");
        _isRunning = false;
    }

    threadInfo(MSG_START, "GUI Initiated, Waiting for stream...");
    for(;;) {
        if (!_canDisplay) continue;
        if (!_guiPtr->update()) break;
        // 1 ms lagging time to display
    }

    _isRunning = false;
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



