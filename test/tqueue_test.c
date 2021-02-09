#include <stdio.h>
#include "../tqueue/tqueue.h"

int main() {
    TQueue queue = NULL;
    long data1 = 756756;
    long data2 = 43543;
    long data3 = 876324;
    int i1 = tqueue_enqueue(&queue, &data1);
    int i2 = tqueue_enqueue(&queue, &data2);
    printf("%lu\n", tqueue_size(queue));
    int i3 = tqueue_enqueue(&queue, &data3);
    printf("%d,%d,%d\n", i1, i2, i3);
    void *data = tqueue_get_data(queue);
    printf("%ld\n",*((long*) data));

    printf("%ld\n",*((long*) tqueue_pop(&queue)));
    i1 = tqueue_enqueue(&queue, &data3);
    printf("%d,%d,%d\n", i1, i2, i3);
    printf("%ld\n",*((long*) tqueue_pop(&queue)));
    printf("%lu\n", tqueue_size(queue));
    TQueue offset = tqueue_at_offset(queue, 1);
    //printf("%ld\n",*((long*) offset->data));
    tqueue_free(&queue);

}
