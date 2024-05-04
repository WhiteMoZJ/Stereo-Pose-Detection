//
// Created by junchau on 10/22/23.
//

#include "thread_task.h"


ThreadTask::ThreadTask():
    _cameraPtr(std::make_unique<device::Camera>()),
    _detectorPtr(std::make_unique<BodyDetector>()),
    _guiPtr(std::make_unique<Gui>()),
    _isShoutdown(false),
    _canDisplay(false),
    _middleBuffer(3),
    _backBuffer(3)
{
    _startTime = getNow();
}

ThreadTask::~ThreadTask()
{
    threadInfo(MSG_END, "Stream End");
    _cameraPtr->endStream();
    threadInfo(MSG_EXIT, "Program Exit(0)");
}

void ThreadTask::init()
{
    std::unique_lock<std::mutex> thread_lock(_mtx);
    if (!_cameraPtr->setUpStream()) {
        threadInfo(MSG_ERR, "No device connect");
        return;
    }

    threadInfo(MSG_START, "Camera Initiated");
    threadInfo(MSG_START, "Start Streaming...");
}

void ThreadTask::produce()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::lock_guard<std::mutex> guard(_mtx);
    std::cout << "produce\n";
    while (!_isShoutdown) {
        if (!_cameraPtr->startStream()) continue;
        std::array<cv::Mat, 2> images;
        *_cameraPtr >> images;

        if (!_middleBuffer.push(Frame{images, _cameraPtr->getFrameCount(), getTimeStamp()}))
            continue;

        // push frame to _frontBuffer for display
        if (_middleBuffer.swapLatestTo(_guiPtr->frontBuffer) && _middleBuffer.swapLatestTo(_backBuffer))
            _canDisplay = true;
    }
}

void ThreadTask::consume()
{
    std::cout << "consume\n";
    while (!_isShoutdown) {
        // TODO: solve 3D pose
        if (!_canDisplay) continue;

        Frame frame;
        if (!_backBuffer.getLatest(frame)) continue;

        if (!_detectorPtr->detectBody(frame)) continue;
        _detectorPtr->solve3D();
    }
}

void ThreadTask::display()
{
    if (!_guiPtr->init("Pose Detection")) {
        threadInfo(MSG_ERR, "GUI Initiated Failed");
        _isShoutdown = true;
    }

    threadInfo(MSG_START, "GUI Initiated, Waiting for stream...");
    while (!_isShoutdown) {
        _isShoutdown = !_guiPtr->update();
        // 1 ms lagging time to display
    }
}

void ThreadTask::threadInfo(const MSGType type, const char *info) const
{
#ifdef DEBUG
    constexpr std::array<const char*, 5> msgs{"START", "END", "EXIT", "WARNING", "ERROR"};
    const int time = static_cast<int>(getTimeStamp());
    if (type > 4) {
        printf("[ INFO@%02d:%02d:%02d:%03d] Message type undefined\n",
               time/3600000, time/60000%60, time/1000%60, time%1000);
    }
    printf("[ INFO@%02d:%02d:%02d.%03d] %s:%s\n",
           time/3600000, time/60000%60, time/1000%60, time%1000, msgs[type], info);
#endif  //DEBUG
}



