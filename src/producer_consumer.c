#include "common.h"
#include "producer_consumer.h"

void bounded_buffer_init(bounded_buffer_t *bb) {
    bb->in = 0;
    bb->out = 0;
    bb->size = 10;
    pthread_mutex_init(&bb->mutex, NULL);
    bb->empty = dispatch_semaphore_create(bb->size);
    bb->full = dispatch_semaphore_create(0);
}

void produce_item(bounded_buffer_t *bb, int item) {
    dispatch_semaphore_wait(bb->empty, DISPATCH_TIME_FOREVER);
    
    pthread_mutex_lock(&bb->mutex);
    bb->buffer[bb->in] = item;
    THREAD_SAFE_PRINT("[PRODUCER] Produced item: %d at position %d\n", item, bb->in);
    bb->in = (bb->in + 1) % bb->size;
    pthread_mutex_unlock(&bb->mutex);
    dispatch_semaphore_signal(bb->full);
}

int consume_item(bounded_buffer_t *bb) {
    int item;
    
    dispatch_semaphore_wait(bb->full, DISPATCH_TIME_FOREVER);
    pthread_mutex_lock(&bb->mutex);
    item = bb->buffer[bb->out];
    THREAD_SAFE_PRINT("[CONSUMER] Consumed item: %d from position %d\n", item, bb->out);
    bb->out = (bb->out + 1) % bb->size;
    pthread_mutex_unlock(&bb->mutex);
    dispatch_semaphore_signal(bb->empty);
    return item;
}

void bounded_buffer_destroy(bounded_buffer_t *bb) {
    pthread_mutex_destroy(&bb->mutex);
    /* No explicit cleanup needed for dispatch_semaphore_t */
}
