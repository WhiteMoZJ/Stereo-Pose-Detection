//
// Created by junchau on 10/22/23.
//

#ifndef STDAFX_H
#define STDAFX_H

#include <mutex>
#include <chrono>
#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <opencv2/imgproc.hpp>
#include <opencv2/dnn.hpp>
#include <Eigen/Dense>

typedef std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<double,std::milli>> tick;
typedef std::array<Eigen::Matrix<float, 15, 2>, 2> PointArray;
typedef std::array<Eigen::Vector3f, 15> SpacePoints;

enum MSGType
{
    MSG_START   = 0,
    MSG_END     = 1,
    MSG_EXIT    = 2,
    MSG_WARN    = 3,
    MSG_ERR     = 4
};

inline tick getNow()
{
    return std::chrono::high_resolution_clock::now();
}


#endif //STDAFX_H
