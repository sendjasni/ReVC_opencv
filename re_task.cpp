#include "re_task.hpp"

cv::Mat frame[3];
int frame_index_write = 0;
int frame_index_read = 1;
int frame_index_read_ = 2;

// bool shutdown = false;

int SwapIndex(int *fi)
{
    return *fi = (*fi + 1) % 3;
}

pthread_mutex_t frame_rw = PTHREAD_MUTEX_INITIALIZER;      /*  thread mutex 
                                                            variable */
pthread_cond_t synch_condition = PTHREAD_COND_INITIALIZER; /*thread condition 
                                                            variable*/
cv::VideoCapture capture(cv::CAP_ANY);                     /* open the default 
                                                            camera */

// int lowThreshold = 0;
// const int max_lowThreshold = 100;
cv::Mat gray_capture;
cv::Mat detected_edges;

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
    rtmode_t my_modes;
    int mode_res = rtmode_init(&my_modes, 2);
    if (mode_res < 0)
    {
        std::cout << "Cannot create the mode manager!" << std::endl;
    }

    checkTaskCreation(StartTask(allocated_processor, &my_modes, CapturingImageTask));
    allocated_processor++;

    checkTaskCreation(StartTask(allocated_processor, &my_modes, DisplyingImageTask));
    allocated_processor++;

    checkTaskCreation(StartTask(allocated_processor, &my_modes, EdgeDetectionTask));
    allocated_processor++;

    // ptask_activate(0);
    // ptask_activate_at(1, PER, MILLI);
    // ptask_activate_at(2, PER, MILLI);

    rtmode_changemode(&my_modes, MODE_ON);

    int exit_char;
    exit_char = getchar();
    while (exit_char != ESC)
    {
        exit_char = getchar();
    }
    rtmode_changemode(&my_modes, MODE_OFF);
    // shutdown = true;
    return allocated_processor;
}

static int StartTask(int processor, rtmode_t *modes, void (*task_body)(void))
{

    tpars params;

    params = TASK_SPEC_DFL;
    params.period = tspec_from(PER, MILLI);
    params.priority = PRIORITY;
    params.modes = modes;
    params.mode_list[0] = MODE_ON;
    params.mode_list[1] = MODE_OFF;
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
    ptask_wait_for_activation();

    int task_job = 0;

    while (1)
    {
        DisplayTasksInstances(task_job);

        pthread_mutex_lock(&frame_rw);

        capture.grab();
        capture.retrieve(frame[frame_index_write], CHANNEL);
        SwapIndex(&frame_index_write);

        pthread_cond_broadcast(&synch_condition); /* After capturing the frame
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
    ptask_wait_for_activation();

    int task_job = 0;

    while (1)
    {
        DisplayTasksInstances(task_job);

        pthread_mutex_lock(&frame_rw);
        pthread_cond_wait(&synch_condition, &frame_rw); /*wait for the capturing
                                                          func to send a signal*/
        if (frame[frame_index_read].data)
        {
            cv::imshow(CAPTURED_IMAGE_WINDOW_NAME, frame[frame_index_read]);
            SwapIndex(&frame_index_read);

            cv::waitKey(1);
        }
        else
        {
            std::cout << "Frame reading error" << std::endl;
        }

        pthread_mutex_unlock(&frame_rw);

        ptask_wait_for_period();
        task_job++;
    }
}

ptask EdgeDetectionTask()
{
    ptask_wait_for_activation();

    int task_job = 0;

    while (1)
    {
        DisplayTasksInstances(task_job);

        pthread_mutex_lock(&frame_rw);
        pthread_cond_wait(&synch_condition, &frame_rw); /*wait for the capturing
                                                          func to send a signal*/
        if (frame[frame_index_read_].data)
        {
            cv::cvtColor(frame[frame_index_read_], gray_capture, cv::COLOR_BGR2GRAY);
            cv::blur(gray_capture, detected_edges, cv::Size(3, 3));
            cv::Canny(detected_edges, detected_edges, 0, 100, 3);

            cv::imshow(EDGE_IMAGE_WINDOW_NAME, detected_edges);
            SwapIndex(&frame_index_read_);

            cv::waitKey(1);
        }
        else
        {
            std::cout << "Frame reading error" << std::endl;
        }

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