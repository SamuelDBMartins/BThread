//
// Created by Samuel Martins on 10.10.20.
//

#ifndef EX05_BTHREAD_PRIVATE_H
#define EX05_BTHREAD_PRIVATE_H

#include <setjmp.h>
#include "bthread.h"

typedef enum { __BTHREAD_READY = 0, __BTHREAD_BLOCKED, __BTHREAD_SLEEPING,
    __BTHREAD_ZOMBIE} bthread_state;

typedef struct {
    bthread_t tid;
    bthread_routine body;
    void* arg;
    bthread_state state;
    bthread_attr_t attr;
    char* stack;
    jmp_buf context;
    void* retval;
} __bthread_private;

#endif //EX05_BTHREAD_PRIVATE_H
