#include "Task/thread_task.h"
#include "stdafx.h"
#include "linux/videodev2.h"
#include <libv4l2.h>

unsigned char           * g_pRgbBuffer;

int main()
{
    ThreadTask thread_task;
    thread_task.init();

    std::thread produce_thread{&ThreadTask::produce, &thread_task};
    std::thread consume_thread{&ThreadTask::consume, &thread_task};
    std::thread display_thread{&ThreadTask::display, &thread_task};

    produce_thread.join();
    consume_thread.join();
    display_thread.join();

    return 0;
}