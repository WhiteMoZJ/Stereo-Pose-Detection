//
// Created by junchau on 11/5/23.
//

#ifndef MV_CAMERA_H
#define MV_CAMERA_H

#include <opencv2/opencv.hpp>
#include <MVSDK/CameraApi.h>

namespace device
{
class MVCamera
{
public:
    MVCamera();
    MVCamera(const MVCamera &) = delete;
    ~MVCamera();

    /**
 * @brief   Initiate video frame
 * @param   width and height
 */
    void setVideoFormat(int width = 640, int height = 480);

    /**
     * @brief   Set Camera Exposure Time
     * @param   t Exposure Time(ms)
     */
    void setExposureTime(double t = 0.f);

    /**
     * @brief   Initiate camera
     * @param   frames per second
     * @return  Is camera start successful
     */
    bool setUpCam();

    // video stream control
    /**
     * @brief   Start camera streaming
     * @return  Is streaming
     */
    bool startStream();

    /**
     * @brief   End camera streaming
     */
    void endStream();

    /**
     * @brief   Get resolution
     * @return  cv::Size(_width, _height)
     */
    cv::Size getResolution() const
    {
        return {_width,_height};
    }

    /**
     * @brief   Get frame count
     * @return  _frameCount(private number variable)
     */
    size_t getFrameCount() const
    {
        return _frameCount;
    }

    void printInfo();

    bool isOpened() const
    {
        return _open;
    }

    MVCamera& operator >> (std::array<cv::Mat, 2> &images);

private:
    int _height, _width;
    double _exposureTime;
    unsigned int _frameCount;       // count of frame
    bool _open;

    unsigned char           * g_pRgbBuffer;

    int                     _cameraCounts;
    int                     _status = -1;
    int                     _camera = 1;
    tSdkCameraDevInfo       _cameraEnumList;
    tSdkCameraCapbility     _capability;      // device info
    tSdkFrameHead           _frameInfo;
    BYTE*			        _pbyBuffer;
    int                     _channel;
};
}


#endif //MV_CAMERA_H
