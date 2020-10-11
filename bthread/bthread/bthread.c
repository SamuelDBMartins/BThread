//
// Created by Samuel Martins on 11.10.20.
//

#include "bthread.h"
#include "bthread_private.h"
#include <stdlib.h>
#include <stdint.h>
#include <zconf.h>

#define save_context(CONTEXT) sigsetjmp(CONTEXT, 1)
#define restore_context(CONTEXT) siglongjmp(CONTEXT, 1)

__bthread_scheduler_private scheduler = {0};

//variabile globale??
__bthread_scheduler_private* bthread_get_scheduler() {
    return &scheduler;
}

//malloc?
//bthread corresponds to the position in the queue
//ready
//return value
int bthread_create(bthread_t *bthread, const bthread_attr_t *attr,
                   void *(*start_routine) (void *), void *arg) {
    __bthread_private *bthread_private = malloc(sizeof(__bthread_private));
    bthread_private->attr = *attr;
    bthread_private->body = start_routine;
    bthread_private->stack = NULL;
    bthread_private->state = __BTHREAD_READY;
    bthread_private->tid = *bthread;
    bthread_private->arg = arg;
    tqueue_enqueue(&bthread_get_scheduler()->queue, bthread_private);
}

void bthread_yield() {
    __bthread_private *thread = tqueue_get_data(bthread_get_scheduler()->current_item);
    if (!save_context(thread->context)) {
        restore_context(bthread_get_scheduler()->context);
    }
}

//retvalue??
void bthread_exit(void *retval) {
    __bthread_private *thread = tqueue_get_data(bthread_get_scheduler()->current_item);
    thread->retval = retval;
    thread->state = __BTHREAD_ZOMBIE;
    bthread_yield();
}


static int bthread_check_if_zombie(bthread_t bthread, void **retval) {
    TQueue node = bthread_get_queue_at(bthread);
    __bthread_private *b = tqueue_get_data(node);

    if (b->state != __BTHREAD_ZOMBIE) {
        return 0;
    }

    if (b->retval != NULL) {
        *retval = b->retval;
    }

    free(b->stack);
    if(b == tqueue_get_data(bthread_get_scheduler()->queue)) {
        tqueue_pop(&bthread_get_scheduler()->queue);
    } else {
        tqueue_pop(&node);
    }

    return 1;
}

static TQueue bthread_get_queue_at(bthread_t bthread) {
    TQueue q = bthread_get_scheduler()->queue;
    TQueue actual_node;

    int size = tqueue_size(q);
    for (int i = 0; i < size; ++i) {
        actual_node = tqueue_at_offset(q, 0);
        __bthread_private *b = tqueue_get_data(actual_node);
        if (b->tid == bthread) {
            return actual_node;
        }
    }
    return NULL;
}

//STACK_SIZE??
//handler zombie threads
int bthread_join(bthread_t bthread, void **retval) {
    volatile __bthread_scheduler_private *scheduler = bthread_get_scheduler();
    scheduler->current_item = scheduler->queue;
    save_context(scheduler->context);
    if (bthread_check_if_zombie(bthread, retval)) return 0;
    __bthread_private *tp;
    do {
        scheduler->current_item = tqueue_at_offset(scheduler->current_item, 1);
        tp = (__bthread_private *) tqueue_get_data(scheduler->current_item);
    } while (tp->state != __BTHREAD_READY);
    if (tp->stack) {
        restore_context(tp->context);
    } else {
        tp->stack = (char *) malloc(sizeof(char) * STACK_SIZE);
        unsigned long target =
                tp->stack + STACK_SIZE - 1;
#if __APPLE__
        // OSX requires 16 bytes aligned stack
        target -= (target % 16);
#endif
#if __x86_64__
        asm __volatile__("movq %0, %%rsp" :: "r"((intptr_t) target));
#else
        asm __volatile__("movl %0, %%esp" :: "r"((intptr_t) target));
#endif
        bthread_exit(tp->body(tp->arg));
    }
}