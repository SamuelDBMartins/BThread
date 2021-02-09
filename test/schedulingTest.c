//
// Created by smartins
//

#include <stdlib.h>

#include "../bthread/bthread.h"

void* mythread(void* arg) {
    int cnt = 0;
    for (int i = 0; i < 1000; ++i) {
        cnt++;
        bthread_sleep((rand() / RAND_MAX) + 0.5);
    }
    return (void *) cnt;
}

int main() {
    bthread_t t1, t2, t3;
    int i1, i2, i3;
    bthread_create(&t1, NULL, mythread, NULL);
    bthread_create(&t2, NULL, mythread, NULL);
    bthread_create(&t3, NULL, mythread, NULL);
    bthread_setPriority(t1, 1000);
    bthread_join(t1, &i1);
    printf("%d ", i1);
    bthread_join(t2, &i2);
    printf("%d ", i2);
    bthread_join(t3, &i3);
    printf("%d ", i3);

    return 0;
}

