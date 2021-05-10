cmake_minimum_required(VERSION 3.5)
project(Distributed_computing C)

set(CMAKE_C_STANDARD 90)

add_executable(Distributed_computing main.c LamportMutex.c LamportMutex.h RicardMutex.c RicardMutex.h)
target_link_libraries(Distributed_computing -lpthread)