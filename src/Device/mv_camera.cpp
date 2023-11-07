//
// Created by junchau on 11/5/23.
//

#include "mv_camera.h"

using namespace device;

MVCamera::MVCamera() :
        settings({0, 1}),
        _frameCount(0),
        _status(0),
        _camera(0),
        _open(false),
        _channel(0)
{
    _width = 0;
    _height = 0;
    CameraSdkInit(1);
}

MVCamera::~MVCamera()
{
    _open = false;
    CameraUnInit(_camera);
    free(g_pRgbBuffer);
}

void MVCamera::setVideoFormat(int width, int height)
{
    if (_width == width && _height == height)
    {
        return;
    }
    _width  = width;
    _height = height;
}


void MVCamera::setExposureTime(float t)
{
    if (settings.exposureTime == t)
    {
        return;
    }
    settings.exposureTime = t;
}

bool MVCamera::setUpCam()
{
    // Initiate camera connection
    _status = CameraEnumerateDevice(&_cameraEnumList,&_cameraCounts);
    // No device connect
    if (_cameraCounts == 0) {
        return false;
    }
    _status = CameraInit(&_cameraEnumList,-1,-1,&_camera);
    if (_status != CAMERA_STATUS_SUCCESS) {
        return false;
    }

    CameraGetCapability(_camera,&_capability);
    // Camera settings
    g_pRgbBuffer = (unsigned char*)malloc(_capability.sResolutionRange.iHeightMax*_capability.sResolutionRange.iWidthMax*3);
    CameraPlay(_camera);

    _open = true;
    return true;
}

bool device::MVCamera::startStream()
{
    CameraSetIspOutFormat(_camera,CAMERA_MEDIA_TYPE_BGR8);
    if (CameraGetImageBuffer(_camera,&_frameInfo,&_pbyBuffer,1000) == CAMERA_STATUS_SUCCESS) {
        CameraImageProcess(_camera, _pbyBuffer, g_pRgbBuffer,&_frameInfo);
        _frameCount++;
        return true;
    }
    return false;
}

void MVCamera::endStream()
{
    // Need to release buffer after getting
    CameraReleaseImageBuffer(_camera,_pbyBuffer);
}

MVCamera &MVCamera::operator >> (std::array<cv::Mat, 2> &images)
{
    cv::Mat frame {cv::Size(_frameInfo.iWidth,_frameInfo.iHeight), CV_8UC3, g_pRgbBuffer};
    // resize frame INTER_NEAREST method could be the fastest
    cv::resize(frame, frame, cv::Size(_width, _height), 0, 0, cv::INTER_NEAREST_EXACT);
    frame.copyTo(images[0]);
    frame.copyTo(images[1]);
    // TODO: I need 2 cameras to build a stereo
    return *this;
}

void MVCamera::changeExposureTime()
{
    if (settings.exposureTime == 0.f)
        //! if set exposure time 0, the camera will be not responding for a while
        CameraSetAeState(_camera, TRUE);
    else {
        CameraSetAeState(_camera, FALSE);
        CameraSetExposureTime(_camera, settings.exposureTime * 1000.);
    }
}

void MVCamera::printInfo()
{

}
