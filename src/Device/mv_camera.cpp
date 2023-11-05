//
// Created by junchau on 11/5/23.
//

#include "mv_camera.h"


device::MVCamera::MVCamera()
{
    _width          = 0;
    _height         = 0;
    _exposureTime   = 0;
    _frameCount     = 0;
    _open           = false;
    _channel        = 3;
    CameraSdkInit(1);
}

device::MVCamera::~MVCamera()
{
    _open = false;
    CameraUnInit(_camera);
    free(g_pRgbBuffer);
}

void device::MVCamera::setVideoFormat(int width, int height)
{
    if (_width == width && _height == height)
    {
        return;
    }
    _width  = width;
    _height = height;
}


void device::MVCamera::setExposureTime(double t)
{
    if (_exposureTime == t)
    {
        return;
    }
    _exposureTime = t;
}

bool device::MVCamera::setUpCam()
{
    // Initiate camera connection
    _status = CameraEnumerateDevice(&_cameraEnumList,&_cameraCounts);
    // No device connect
    if(_cameraCounts == 0){
        return false;
    }
    _status = CameraInit(&_cameraEnumList,-1,-1,&_camera);
    if(_status != CAMERA_STATUS_SUCCESS){
        return false;
    }

    CameraGetCapability(_camera,&_capability);
    // Camera settings
    g_pRgbBuffer = (unsigned char*)malloc(_capability.sResolutionRange.iHeightMax*_capability.sResolutionRange.iWidthMax*3);
    CameraPlay(_camera);

    if (_exposureTime != 0.f) {
        CameraSetAeState(_camera, FALSE);
        CameraSetExposureTime(_camera, _exposureTime * 1000.);
    }

    CameraSetIspOutFormat(_camera,CAMERA_MEDIA_TYPE_BGR8);

    _open = true;
    return true;
}

bool device::MVCamera::startStream()
{
    if(CameraGetImageBuffer(_camera,&_frameInfo,&_pbyBuffer,1000) == CAMERA_STATUS_SUCCESS)
    {
        CameraImageProcess(_camera, _pbyBuffer, g_pRgbBuffer,&_frameInfo);
        _frameCount++;
        return true;
    }
    return false;
}

void device::MVCamera::endStream()
{
    CameraReleaseImageBuffer(_camera,_pbyBuffer);
}

device::MVCamera &device::MVCamera::operator >> (std::array<cv::Mat, 2> &images)
{
    cv::Mat frame {cv::Size(_frameInfo.iWidth,_frameInfo.iHeight), CV_8UC3, g_pRgbBuffer};
    // resize frame
    cv::resize(frame, frame, cv::Size(_width, _height), 0, 0, cv::INTER_AREA);
    frame.copyTo(images[0]);
    frame.copyTo(images[1]);
    return *this;
}

