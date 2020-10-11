cmake_minimum_required(VERSION 3.17)
project(ex05 C)

set(CMAKE_C_STANDARD 11)

add_executable(ex05 main.c bthread_private.h bthread.h)