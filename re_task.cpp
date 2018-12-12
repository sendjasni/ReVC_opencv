#include <iostream>
#include <cstdlib>
#include "opencv2/opencv.hpp"

#include <ptask.h>
#include <tstat.h>

#define PER 50
#define ESC 27

int checkTaskCreation(int task_index);
int ImageCapture();
static int StartTask(int processor, void (*task_body)(void));
void TasksStatisticComputing(int nbr_tasks);
ptask CapturingImageTask();
int TaskCreat();

int main(int argc, char const *argv[])
{
    // Initiate the PTask
    ptask_init(SCHED_FIFO, PARTITIONED, NO_PROTOCOL);

    // Task creation
    int created_tasks = 0; /* total number of created tasks*/
    created_tasks = TaskCreat();

    // Compute the task's related stats
    TasksStatisticComputing(created_tasks);

    return EXIT_SUCCESS;
}

int TaskCreat()
{
    int allocated_processor = 0;
    checkTaskCreation(StartTask(allocated_processor, CapturingImageTask));
    allocated_processor++;

    int exit_char;
    exit_char = getchar();
    while (exit_char != ESC)
    {
        exit_char = getchar();
    }

    return allocated_processor;
}

static int StartTask(int processor, void (*task_body)(void))
{
    tpars params;

    params = TASK_SPEC_DFL;
    params.period = tspec_from(PER, MILLI);
    params.priority = PRIORITY;
    params.measure_flag = 1;
    params.act_flag = NOW;
    params.processor = processor;

    return ptask_create_param(task_body, &params);
}

/*
  Checking the tasks creation status
    */
int checkTaskCreation(int task_index)
{
    if (task_index != -1)
    {
        std::cout << "The task %d was created successfully \n"
                  << task_index
                  << std::endl;
        return EXIT_SUCCESS;
    }
    else
    {
        std::cout << " ERROR! task not created !!! \n"
                  << std::endl;
        exit(EXIT_FAILURE);
    }
}

/* 
    The capturing task' body
    */

ptask CapturingImageTask()
{
    int task_job = 0;

    while (1)
    {
         std::cout << "The job %d of Task T%d is running on core %d at time : %ld\n"
                    << task_job << ptask_get_index() << sched_getcpu() 
                    << ptask_gettime(MILLI) << std::endl;

         ImageCapture();
         // SwapBuffer(front_buffer, back_buffer);
         ptask_wait_for_period();
         task_job++;
    }
}

int ImageCapture()
{
    cv::VideoCapture capture(cv::CAP_ANY);

    if (!capture.isOpened())
    {
        std::cout << "Error opening video camera! " << std::endl;
        return EXIT_FAILURE;
    }

    cv::Mat frame;
    capture >> frame;
    cv::imshow("this is you, smile! :)", frame);
    cv::waitKey(PER);
    capture.release();

    return EXIT_SUCCESS;
}
/* 
   Compute the worst case execution and average time
   for the created tasks
*/

void TasksStatisticComputing(int nbr_tasks)
{
    for (int task_index = 0; task_index < nbr_tasks; task_index++)
    {
        tspec wcet;
        wcet = ptask_get_wcet(task_index);

        tspec avg;
        avg = ptask_get_avg(task_index);

         std::cout << "Task %d : WCET = %ld\t AVG = %ld\t NINST=%d \n"
                    << task_index << tspec_to(&wcet, MILLI) << tspec_to(&avg, MILLI)
                    << ptask_get_numinstances(task_index) << std::endl;
    }
}
