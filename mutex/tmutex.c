//
// Created by Samuel Martins on 24.12.20.
//

#include <assert.h>
#include <stdio.h>
#include "tmutex.h"
#include "../bthread/bthread_private.h"

int bthread_mutex_init(bthread_mutex_t *m, const bthread_mutexattr_t *attr) {
    assert(m != NULL);
    m->owner = NULL;
    m->waiting_list = NULL;
    return 0;
}

int bthread_mutex_destroy(bthread_mutex_t *m) {
    assert(m->owner == NULL);
    assert(tqueue_size(m->waiting_list) == 0);
    return 0;
}

int bthread_mutex_lock(bthread_mutex_t *m) {
    bthread_block_timer_signal();
    __bthread_scheduler_private *scheduler = bthread_get_scheduler();
    volatile __bthread_private *bthread = (__bthread_private *) tqueue_get_data(scheduler->current_item);
    if (m->owner == NULL) {
        m->owner = bthread;
        bthread_unblock_timer_signal();
    } else {
        bthread->state = __BTHREAD_BLOCKED;
        tqueue_enqueue(&m->waiting_list, bthread);
        while (bthread->state != __BTHREAD_READY) {
            bthread_yield();
        };
    }
    return 0;
}

int bthread_mutex_trylock(bthread_mutex_t *m) {
    bthread_block_timer_signal();
    __bthread_scheduler_private *scheduler = bthread_get_scheduler();
    __bthread_private *bthread = (__bthread_private *) tqueue_get_data(scheduler->current_item);

    if (m->owner == NULL) {
        m->owner = bthread;
        bthread_unblock_timer_signal();
    } else {
        bthread_unblock_timer_signal();
        return -1;
    }
    return 0;
}

int bthread_mutex_unlock(bthread_mutex_t *m) {
    bthread_block_timer_signal();
    assert(m->owner != NULL);
    __bthread_scheduler_private *scheduler = bthread_get_scheduler();
    assert(m->owner == tqueue_get_data(scheduler->current_item));
    __bthread_private *unlock = tqueue_pop(&m->waiting_list);
    if (unlock != NULL) {
        m->owner = unlock;
        unlock->state = __BTHREAD_READY;
        bthread_yield();
        return 0;
    } else {
        m->owner = NULL;
    }
    bthread_unblock_timer_signal();
    return 0;
}