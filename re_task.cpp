#include "re_task.hpp"

cv::Mat frame_1;
// cv::Mat frame_2;

pthread_mutex_t frame_rw = PTHREAD_MUTEX_INITIALIZER;      /*  thread mutex 
                                                            variable */
pthread_cond_t synch_condition = PTHREAD_COND_INITIALIZER; /*thread condition 
                                                            variable*/
cv::VideoCapture capture(cv::CAP_ANY);                     /* open the default 
                                                            camera */

// int lowThreshold = 0;
// const int max_lowThreshold = 100;
// const int ratio = 3;
// const int kernel_size = 3;
// const char *second_window_name = "Edge Map";
// cv::Mat gray_capture;
// cv::Mat detected_edges;

void CheckCapturingDevice()
{
    if (!capture.isOpened()) /* check if the camera device is open */
    {
        std::cout << "Error opening video camera! " << std::endl;
        exit(EXIT_FAILURE);
    }
}

int TaskCreat()
{
    int allocated_processor = 0;

    checkTaskCreation(StartTask(allocated_processor, CapturingImageTask));
    allocated_processor++;

    checkTaskCreation(StartTask(allocated_processor, DisplyingImageTask));
    allocated_processor++;

    // checkTaskCreation(StartTask(allocated_processor, FilterApplyingTask));
    // allocated_processor++;

    ptask_activate(0);
    ptask_activate_at(1, PER, MILLI);
    // ptask_activate_at(2, PER * 2, MILLI);

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
    Display runing tasks information
    */

void DisplayTasksInstances(int job_id)
{
    std::cout << "The job " << job_id << " of Task T" << ptask_get_index()
              << " is running on core " << sched_getcpu() << " at time : "
              << ptask_gettime(MILLI) << std::endl;
}

/* 
    The capturing task' body
    */

ptask CapturingImageTask()
{

    int task_job = 0;

    while (1)
    {
        DisplayTasksInstances(task_job);

        pthread_mutex_lock(&frame_rw);
        capture.grab();
        // std::swap(frame_1, frame_2);
        pthread_cond_signal(&synch_condition); /* After capturing the frame 
                                                    signal the displaying task*/
        pthread_mutex_unlock(&frame_rw);

        ptask_wait_for_period();
        task_job++;
    }
}

/* 
    The displaying task' body
    */

ptask DisplyingImageTask()
{

    int task_job = 0;

    while (1)
    {
        DisplayTasksInstances(task_job);

        pthread_mutex_lock(&frame_rw);
        pthread_cond_wait(&synch_condition, &frame_rw); /*wait for the capturing
                                                         func to send a signal*/
        capture.retrieve(frame_1, CHANNEL);
        if (frame_1.data)
        {
            cv::imshow(CAPTURED_IMAGE_WINDOW_NAME, frame_1);
            // std::swap(frame_2, frame_1);
            // frame_2 = frame_1;
            cv::waitKey(1);
        }
        pthread_mutex_unlock(&frame_rw);
        ptask_wait_for_period();
        task_job++;
    }
}

// ptask FilterApplyingTask()
// {
//     int task_job = 0;

//     while (1)
//     {
//         DisplayTasksInstances(task_job);
//         cv::cvtColor(frame_2, gray_capture, cv::COLOR_BayerGB2GRAY);
//         cv::blur(gray_capture, detected_edges, cv::Size(3, 3));
//         cv::Canny(detected_edges, detected_edges, lowThreshold, lowThreshold * ratio, kernel_size);
//         cv::imshow(second_window_name, detected_edges);
//         ptask_wait_for_period();
//         task_job++;
//     }
// }

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
