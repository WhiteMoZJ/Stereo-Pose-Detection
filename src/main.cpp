#include <iostream>
#include "Task/thread_task.h"


int main()
{
    ThreadTask thread_task;
    thread_task.init();

    std::thread produce_thread{&ThreadTask::produce, &thread_task};
    std::thread consume_thread{&ThreadTask::consume, &thread_task};

    produce_thread.join();
    consume_thread.join();

    return 0;
}