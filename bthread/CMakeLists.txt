cmake_minimum_required(VERSION 3.17)
project(bthread C)

set(CMAKE_C_STANDARD 11)

add_executable(bthread main.c bthread/bthread.h)