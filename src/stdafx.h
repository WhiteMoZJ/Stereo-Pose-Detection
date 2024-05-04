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
#include <condition_variable>
#include <opencv2/opencv.hpp>

typedef std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<double,std::milli>> timer;

inline timer getNow()
{
    return std::chrono::high_resolution_clock::now();
}

enum MSGType
{
    MSG_START   = 0,
    MSG_END     = 1,
    MSG_EXIT    = 2,
    MSG_WARN    = 3,
    MSG_ERR     = 4
};


#endif //STDAFX_H
