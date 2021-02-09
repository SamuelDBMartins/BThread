//
// Created by Samuel Martins on 24.12.20.
//

#ifndef BTHREAD_TMUTEX_H
#define BTHREAD_TMUTEX_H

#include "../tqueue/tqueue.h"

typedef struct {
    void *owner;
    TQueue waiting_list;
} bthread_mutex_t;

// Defined only for "compatibility" with pthread
typedef struct {
} bthread_mutexattr_t;

// attr is ignored
int bthread_mutex_init(bthread_mutex_t *m, const bthread_mutexattr_t *attr);

int bthread_mutex_destroy(bthread_mutex_t *m);

int bthread_mutex_lock(bthread_mutex_t *m);

int bthread_mutex_trylock(bthread_mutex_t *m);

int bthread_mutex_unlock(bthread_mutex_t *m);


#endif //BTHREAD_TMUTEX_H
