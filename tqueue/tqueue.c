//
// Created by smartins on 28.09.20.
//

#include <stdio.h>
#include <stdlib.h>
#include "tqueue.h"

typedef struct TQueueNode {
    struct TQueueNode* next;
    void* data;
} TQueueNode;

unsigned long int tqueue_enqueue(TQueue* q, void* data){
    TQueueNode *actual_node = *q;
    int i = 0;

    //create node
    TQueueNode *new = malloc(sizeof(TQueueNode));
    new->data = data;

    // queue empty
    if (actual_node == NULL) {
        *q = new;
        (*q)->next = *q;
        return i;
    }

    i++;
    while(actual_node->next != *q) {
        i++;
        actual_node = actual_node->next;
    }

    actual_node->next = new;
    new->next = *q;
    return i;
}

void* tqueue_pop(TQueue* q) {
    if (*q == NULL)
        return NULL;

    TQueue to_delete = *q;
    void *to_return = (*q)->data;

    TQueueNode *actual_node = *q;
    while(actual_node->next != *q) {
        actual_node = actual_node->next;
    }

    if (*q == (*q)->next) {
        *q = NULL;
    } else {
        *q = (*q)->next;
        actual_node->next = *q;
    }
    free(to_delete);

    return to_return;
}

unsigned long int tqueue_size(TQueue q) {
    if (q == NULL)
        return 0;

    TQueueNode *actual_node = q;
    int i = 1;

    while (actual_node->next != q) {
        actual_node = actual_node->next;
        i++;
    }
    return i;
}

TQueue tqueue_at_offset(TQueue q, unsigned long int offset) {
    if (q == NULL)
        return NULL;

    TQueueNode *actual_node = q;
    for (int i = 0; i < offset; ++i) {
        actual_node = actual_node->next;
    }
    return actual_node;
}

void* tqueue_get_data(TQueue q) {
    if (q == NULL)
        return NULL;
    return q->data;
}

void tqueue_free(TQueue *q) {
    int size = tqueue_size(*q);
    for (int i = 0; i < size; ++i) {
        tqueue_pop(q);
    }
}