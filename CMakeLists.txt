cmake_minimum_required(VERSION 3.17)
project(bthread C)

set(CMAKE_C_STANDARD 11)

add_executable(bthread bthread/bthread.h bthread/bthread.c tqueue/tqueue.c tqueue/tqueue.h bthread/bthread_private.h test/schedulingTest.c)
