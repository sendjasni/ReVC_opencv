cmake_minimum_required(VERSION 3.1)
project(videoCapture)
include_directories(.)

set(CMAKE_GXX_FLAGS "-Wall -Wextra -Wconversion  -pedantic -std=gnu11")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(OpenCV REQUIRED)
find_package(ptask REQUIRED)

set(ptask_INCLUDE_DIRS "${ptask_DIR}/include")
message("Include dir ${ptask_INCLUDE_DIRS}")

include_directories ("${ptask_INCLUDE_DIRS}")
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${ALLEGRO_INCLUDE_DIR})

add_executable(revc_opencv main.cpp re_task.cpp)
target_link_libraries(revc_opencv ${OpenCV_LIBS} ptask Threads::Threads rt m)
