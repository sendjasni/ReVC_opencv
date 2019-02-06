#include <iostream>
#include <cstdlib>
#include "opencv2/opencv.hpp"

extern "C"
{
#include <ptask.h>
#include <tstat.h>
#include <time.h>

}

#define PER 50
#define ESC 27
#define PRIORITY 10

cv::Mat frame;
cv::VideoCapture capture(cv::CAP_ANY); /* open the default camera */
pthread_mutex_t frame_rw; /*  PTHREAD_MUTEX_INITIALIZER */

int checkTaskCreation(int task_index);
static int StartTask(int processor, void (*task_body)(void));
void TasksStatisticComputing(int nbr_tasks);
ptask CapturingImageTask();
ptask DisplyingImageTask();
int TaskCreat();

int main(int argc, char const *argv[])
{

    if (!capture.isOpened())            /* check if the camera device is open */ 
    {
        std::cout << "Error opening video camera! " << std::endl;
        exit(EXIT_FAILURE);
    }
   
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

    checkTaskCreation(StartTask(allocated_processor, DisplyingImageTask));
    allocated_processor++;

    ptask_activate(0);
    ptask_activate_at(1, PER, MILLI);

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
    params.act_flag = DEFERRED;
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
        std::cout << "The task " << task_index << " was created successfully"
                  << std::endl;
        return EXIT_SUCCESS;
    }
    else
    {
        std::cout << " ERROR! task not created !!!" << std::endl;
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
        std::cout << "The job " << task_job << " of Task T" << ptask_get_index()
                  << " is running on core " << sched_getcpu() << " at time : "
                  << ptask_gettime(MILLI) << std::endl;

        pthread_mutex_lock(&frame_rw);
        capture.read(frame);
        pthread_mutex_unlock(&frame_rw);

        ptask_wait_for_period();
        task_job++;
    }

}

ptask DisplyingImageTask()
{
     
    int task_job = 0;

    while (1)
    {
        std::cout << "The job " << task_job << " of Task T" << ptask_get_index()
                  << " is running on core " << sched_getcpu() << " at time : "
                  << ptask_gettime(MILLI) << std::endl;

        cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
        
        pthread_mutex_lock(&frame_rw);
        
        cv::imshow("Display window", frame);        
        cv::waitKey(1);
        
        pthread_mutex_unlock(&frame_rw);

        ptask_wait_for_period();
        task_job++;
    }

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

        std::cout << "Task " << task_index << " : WCET = " << tspec_to(&wcet, MILLI)
                  << " AVG = " << tspec_to(&avg, MILLI) << " NINST = "
                  << ptask_get_numinstances(task_index) << std::endl;
    }
}
