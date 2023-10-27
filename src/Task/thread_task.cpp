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
    _cameraPtr->setUpStream();
    threadInfo("START", "Camera Initiated");
}

void ThreadTask::produce()
{
    threadInfo("START", "Start Streaming (Press ESC to exit)");

    while (cv::waitKey(1) != 27) {
        if (!_consumeSignal) break;

        std::unique_lock<std::mutex> lock_frame(_mFrame);

        _cameraPtr->startStream();
        std::array<cv::Mat, 2> images;

        *_cameraPtr >> images;
        _produceSignal = _frameBuffer.push(Frame{{images[0], images[1]}, _cameraPtr->getFrameCount(), getTimeStamp()});
        _cv.notify_all();
    }
    _produceSignal = false;
}

void ThreadTask::consume()
{
    while (cv::waitKey(1) != 27) {
        if (!_produceSignal) break;

        std::unique_lock<std::mutex> lock_frame(_mFrame);
        if (_frameBuffer.empty()) {
            _cv.wait(lock_frame);
        }

        Frame frame;
        if (!_frameBuffer.getLatest(frame)) {
            // Sometimes this thread will read buffer twice before next pushing
            continue;
        }
        _displayFrame = frame;


        lock_frame.unlock();    // Unlock in advance

//        std::unique_lock<std::mutex> lock_points(_mPoint);
//        PointSet point_set;

        // !ERROR openpose needs RGB frame but there's grey only
//        if (!frame.empty())
//            _detectorPtr->detectBody(frame, point_set);
    }
    _consumeSignal = false;
}

void ThreadTask::display()
{
    while (cv::waitKey(1) != 27) {
        if (!_produceSignal || !_consumeSignal) break;

        std::unique_lock<std::mutex> lock_frame(_mFrame);
        if (_displayFrame.empty()) {
            continue;
        }

        Tool::displayCameraFrame(_displayFrame);
        lock_frame.unlock();

    }

    _produceSignal = false;
    _consumeSignal = false;
}



