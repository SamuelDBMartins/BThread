//
// Created by Samuel Martins on 11.10.20.
//

#ifndef BTHREAD_BTHREAD_PRIVATE_H
#define BTHREAD_BTHREAD_PRIVATE_H

#include <setjmp.h>
#include "bthread.h"
#include "../tqueue/tqueue.h"

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

typedef struct {
    TQueue queue;
    TQueue current_item;
    jmp_buf context;
    bthread_t current_tid;
} __bthread_scheduler_private;

__bthread_scheduler_private* bthread_get_scheduler();

void bthread_cleanup();

static int bthread_check_if_zombie(bthread_t bthread, void **retval);

static TQueue bthread_get_queue_at(bthread_t bthread);

#endif //BTHREAD_BTHREAD_PRIVATE_H
