#include "re_task.hpp"

int main(int argc, char const *argv[])
{

    CheckCapturingDevice();

    // Initiate the PTask
    ptask_init(SCHED_FIFO, PARTITIONED, NO_PROTOCOL);

    cv::namedWindow(CAPTURED_IMAGE_WINDOW_NAME, cv::WINDOW_AUTOSIZE);

    // Task creation
    int created_tasks = 0; /* total number of created tasks*/
    created_tasks = TaskCreat();

    // Compute the task's related stats
    TasksStatisticComputing(created_tasks);

    return EXIT_SUCCESS;
}
