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
#define CHANNEL 0
#define CAPTURED_IMAGE_WINDOW_NAME "Original capture"


void CheckCapturingDevice();
int checkTaskCreation(int);
static int StartTask(int, void (*task_body)(void));
void TasksStatisticComputing(int);
void DisplayTasksInstances(int);
ptask CapturingImageTask();
ptask DisplyingImageTask();
ptask FilterApplyingTask();
int TaskCreat();