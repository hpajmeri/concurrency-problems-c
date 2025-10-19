#ifndef READERS_WRITERS_H
#define READERS_WRITERS_H

#include <pthread.h>
#include <dispatch/dispatch.h>

typedef struct {
    int data;
    int read_count;
    pthread_mutex_t read_count_mutex;
    dispatch_semaphore_t write_semaphore;
} shared_resource_t;

void shared_resource_init(shared_resource_t *resource);

void reader_entry(shared_resource_t *resource);

void reader_exit(shared_resource_t *resource);

void writer_entry(shared_resource_t *resource);

void writer_exit(shared_resource_t *resource);

int read_data(shared_resource_t *resource);

void write_data(shared_resource_t *resource, int value);

void shared_resource_destroy(shared_resource_t *resource);

#endif /* READERS_WRITERS_H */
