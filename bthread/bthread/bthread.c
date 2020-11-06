//
// Created by Samuel Martins on 11.10.20.
//

#include "bthread.h"
#include "bthread_private.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <sys/time.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>

#define STACK_SIZE (1 << 16)
#define save_context(CONTEXT) sigsetjmp(CONTEXT, 1)
#define restore_context(CONTEXT) siglongjmp(CONTEXT, 1)
#define QUANTUM_USEC 100

__bthread_scheduler_private mainScheduler = {0};
bthread_t cnt = 0;

double get_current_time_millis() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
}

void bthread_block_timer_signal() {
    sigset_t sigsetNew;
    sigemptyset(&sigsetNew);
    sigaddset(&sigsetNew, SIGVTALRM);
    sigprocmask(SIG_BLOCK, &sigsetNew, NULL);
}

void bthread_unblock_timer_signal() {
    sigset_t sigsetNew;
    sigemptyset(&sigsetNew);
    sigaddset(&sigsetNew, SIGVTALRM);
    sigprocmask(SIG_UNBLOCK, &sigsetNew, NULL);
}

void bthread_printf(const char *format, ...) // requires stdlib.h and stdarg.h
{
    bthread_block_timer_signal();
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    bthread_unblock_timer_signal();
}

__bthread_scheduler_private *bthread_get_scheduler() {
    return &mainScheduler;
}

int bthread_create(bthread_t *bthread, const bthread_attr_t *attr,
                   void *(*start_routine)(void *), void *arg) {
    __bthread_private *bthread_private = malloc(sizeof(__bthread_private));
    bthread_private->attr = *attr;
    bthread_private->body = start_routine;
    bthread_private->stack = NULL;
    bthread_private->state = __BTHREAD_READY;
    bthread_private->tid = cnt++;
    bthread_private->arg = arg;
    *bthread = bthread_private->tid;
    return tqueue_enqueue(&bthread_get_scheduler()->queue, bthread_private);
}


void bthread_yield() {
    __bthread_private *thread = tqueue_get_data(bthread_get_scheduler()->current_item);
    bthread_block_timer_signal();
    if (!save_context(thread->context)) {
        restore_context(bthread_get_scheduler()->context);
    }
    bthread_unblock_timer_signal();
}

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
    if (b == tqueue_get_data(bthread_get_scheduler()->queue)) {
        tqueue_pop(&bthread_get_scheduler()->queue);
    } else {
        tqueue_pop(&node);
    }
    return 1;
}

static TQueue bthread_get_queue_at(bthread_t bthread) {
    TQueue q = bthread_get_scheduler()->queue;
    TQueue actual_node = q;

    if (q == NULL)
        return NULL;

    do {
        __bthread_private *b = tqueue_get_data(actual_node);
        if (b->tid == bthread) {
            return actual_node;
        }
        actual_node = tqueue_at_offset(actual_node, 1);
    } while (actual_node != q);
    return NULL;
}


static void bthread_setup_timer() {
    static bool initialized = false;

    if (!initialized) {
        signal(SIGVTALRM, (void (*)()) bthread_yield);
        struct itimerval time;
        time.it_interval.tv_sec = 0;
        time.it_interval.tv_usec = QUANTUM_USEC;
        time.it_value.tv_sec = 0;
        time.it_value.tv_usec = QUANTUM_USEC;
        initialized = true;
        setitimer(ITIMER_VIRTUAL, &time, NULL);
    }
}

int bthread_join(bthread_t bthread, void **retval) {
    volatile __bthread_scheduler_private *scheduler = bthread_get_scheduler();
    scheduler->current_item = scheduler->queue;
    save_context(scheduler->context);
    if (bthread_check_if_zombie(bthread, retval)) return 0;
    __bthread_private *tp;
    do {
        scheduler->current_item = tqueue_at_offset(scheduler->current_item, 1);
        tp = (__bthread_private *) tqueue_get_data(scheduler->current_item);

        if (tp->state == __BTHREAD_SLEEPING && get_current_time_millis() > tp->wake_up_time) {
            tp->state = __BTHREAD_READY;
        }
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
        asm __volatile__("movq %0, %%rsp"::"r"((intptr_t) target));
#else
        asm __volatile__("movl %0, %%esp" :: "r"((intptr_t) target));
#endif
        bthread_setup_timer();
        bthread_exit(tp->body(tp->arg));
    }
}

void bthread_sleep(double ms) {
    TQueue node = bthread_get_scheduler()->current_item;
    __bthread_private *thread = tqueue_get_data(node);
    thread->wake_up_time = get_current_time_millis() + ms;
    thread->state = __BTHREAD_SLEEPING;
    bthread_yield();
}

int bthread_cancel(bthread_t bthread) {
    TQueue node = bthread_get_queue_at(bthread);
    __bthread_private *thread = tqueue_get_data(node);
    thread->cancel_req = 1;
    return -1;
}

void bthread_testcancel() {
    TQueue node = bthread_get_scheduler()->current_item;
    __bthread_private *thread = tqueue_get_data(node);
    if (thread->cancel_req)
        bthread_exit(NULL);
}

