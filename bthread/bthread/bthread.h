//
// Created by Samuel Martins on 11.10.20.
//

#ifndef BTHREAD_BTHREAD_H
#define BTHREAD_BTHREAD_H

#include <stdio.h>

typedef unsigned long int bthread_t;

typedef struct {
} bthread_attr_t;

typedef void *(*bthread_routine) (void *);

int bthread_create(bthread_t *bthread, const bthread_attr_t *attr,
void *(*start_routine) (void *), void *arg);

int bthread_join(bthread_t bthread, void **retval);

void bthread_yield();

void bthread_exit(void *retval);

void bthread_sleep(double ms);

void bthread_setPriority(bthread_t bthread, int priority);

int bthread_cancel(bthread_t bthread);

void bthread_setScheduling(int i);

#endif //BTHREAD_BTHREAD_H
