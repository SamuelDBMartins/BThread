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
    while(actual_node != NULL && actual_node->next != NULL) {
        actual_node = actual_node->next;
    }

    TQueueNode *new = malloc(sizeof(TQueueNode) * 1);
    new->data = data;
    actual_node->next = new;
}



