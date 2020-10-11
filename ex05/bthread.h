//
// Created by Samuel Martins on 10.10.20.
//

#ifndef EX05_BTHREAD_H
#define EX05_BTHREAD_H

typedef unsigned long int bthread_t;

typedef struct {
} bthread_attr_t;

typedef void *(*bthread_routine) (void *);

#endif //EX05_BTHREAD_H
