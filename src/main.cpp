#include "Task/thread_task.h"


int main()
{
    ThreadTask thread_task;

    std::thread init_thread{&ThreadTask::init, &thread_task};
    std::thread produce_thread{&ThreadTask::produce, &thread_task};
    std::thread consume_thread{&ThreadTask::consume, &thread_task};
    std::thread display_thread{&ThreadTask::display, &thread_task};

    init_thread.join();
    produce_thread.join();
    consume_thread.join();
    display_thread.join();

    return 0;
}