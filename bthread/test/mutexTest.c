#include "../bthread/bthread.h"
#include "../mutex/tmutex.h"

static bthread_mutex_t mutex;

void* mythread(void* arg) {
    bthread_mutex_lock(&mutex);
    bthread_sleep(3000);
    bthread_mutex_unlock(&mutex);
}

int main() {
    bthread_t t1, t2;
    bthread_mutex_init(&mutex, NULL);
    bthread_create(&t1, NULL, mythread, NULL);
    bthread_create(&t2, NULL, mythread, NULL);
    bthread_join(t1, NULL);
    bthread_join(t2, NULL);
    bthread_mutex_destroy(&mutex);
    return 0;
}
