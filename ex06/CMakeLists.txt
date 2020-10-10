cmake_minimum_required(VERSION 3.17)
project(ex06 C)

set(CMAKE_C_STANDARD 11)

add_executable(ex06 main.c tqueue.h tqueue.c)