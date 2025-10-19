#ifndef PRODUCER_CONSUMER_H
#define PRODUCER_CONSUMER_H

#include <pthread.h>
#include <dispatch/dispatch.h>

typedef struct {
    int buffer[10];
    int in;
    int out;
    int size;
    pthread_mutex_t mutex;
    dispatch_semaphore_t empty;
    dispatch_semaphore_t full;
} bounded_buffer_t;

void bounded_buffer_init(bounded_buffer_t *bb);

void produce_item(bounded_buffer_t *bb, int item);

int consume_item(bounded_buffer_t *bb);

void bounded_buffer_destroy(bounded_buffer_t *bb);

#endif /* PRODUCER_CONSUMER_H */
