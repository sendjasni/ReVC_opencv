#include <iostream>
#include <cstdlib>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"

extern "C"
{
#include <ptask.h>
#include <tstat.h>
#include <time.h>
}

#define PER 20
#define ESC 27
#define PRIORITY 10
#define CHANNEL 0
#define CAPTURED_IMAGE_WINDOW_NAME "Original capture"
#define EDGE_IMAGE_WINDOW_NAME "Edge image"

void CheckCapturingDevice();
int checkTaskCreation(int);
static int StartTask(int, void (*task_body)(void));
void TasksStatisticComputing(int);
void DisplayTasksInstances(int);
ptask CapturingImageTask();
ptask DisplyingImageTask();
ptask EdgeDetectionTask();
int TaskCreat();