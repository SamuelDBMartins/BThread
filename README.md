# BThread
Simple implementation of the POSIX Thread Library at user level. This software is compatible with pthread.

## Setup
You can include the files from this project as source files in your project.

## API

Choose scheduling method:
* 0: round-robin
* 1: random
```c
void bthread_scheduling(int i);
```

Create thread
```c
int bthread_create(bthread_t *bthread, const bthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
```

Set priority
```c
void bthread_set_priority(bthread_t bthread, int priority);
```

Wait for thread to finish
```c
int bthread_join(bthread_t bthread, void **retval);
```

Put thread at sleep
```c
void bthread_sleep(double ms);
```

Yield active thread
```c
void bthread_yield();
```

Request cancellation hread
```c
int bthread_cancel(bthread_t bthread);
```

## Features

- Scheduling cooperative or preemptive
- Round-robin with or without priority
- Random scheduling


License
----

MPL-2.0 © Samuel Martins

