# ReVC_opencv           [![Codacy Badge](https://api.codacy.com/project/badge/Grade/16f57a015f5d447b9ce1ce8042d2af1e)](https://app.codacy.com/app/sendjasni/ReVC_opencv?utm_source=github.com&utm_medium=referral&utm_content=sendjasni/ReVC_opencv&utm_campaign=Badge_Grade_Dashboard)

## Contents

- Introduction
- Dependencies
- Build
- License

## Introduction

**Real-time Video Capture** is a simple application for capturing periodicly images from a camera and applying some computer vision processing on it. The [**PTask**](https://github.com/glipari/ptask) lib were used to ensure task-related real-times properties.  

## Dependencies

- cmake >= 3.5
  - All OSes : [click here for installation instructions](https://cmake.org/install/)
- make >= 3.1
  - Linux : make is installed by default on most Linux distributions
- PTask
  - Periodic Real-Time Task interface facility to pthreads, [click here for installation instructions](https://github.com/glipari/ptask#compiling)
- OpenCv
  - Linux :  [click here for installation instructions](https://docs.opencv.org/3.4/d7/d9f/tutorial_linux_install.html)

## Basic Build Instructions

CMake is used as its build system for generating makefiles.

```shell
git clone https://github.com/sendjasni/ReVC_opencv.git
mkdir build && cd build
cmake .. && make
sudo ./revc_opencv
```

## License

ReVC_opencv is open source software [licensed as GNU](https://github.com/sendjasni/ReVC_opencv/blob/master/LICENSE.md).
