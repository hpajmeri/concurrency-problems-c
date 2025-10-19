#include "common.h"
#include "readers_writers.h"

void shared_resource_init(shared_resource_t *resource) {
    resource->data = 0;
    resource->read_count = 0;
    pthread_mutex_init(&resource->read_count_mutex, NULL);
    resource->write_semaphore = dispatch_semaphore_create(1);
}

void reader_entry(shared_resource_t *resource) {
    pthread_mutex_lock(&resource->read_count_mutex);
    resource->read_count++;
    if (resource->read_count == 1) {
        dispatch_semaphore_wait(resource->write_semaphore, DISPATCH_TIME_FOREVER);
    }
    pthread_mutex_unlock(&resource->read_count_mutex);
}

void reader_exit(shared_resource_t *resource) {
    pthread_mutex_lock(&resource->read_count_mutex);
    resource->read_count--;
    if (resource->read_count == 0) {
        dispatch_semaphore_signal(resource->write_semaphore);
    }
    pthread_mutex_unlock(&resource->read_count_mutex);
}

void writer_entry(shared_resource_t *resource) {
    dispatch_semaphore_wait(resource->write_semaphore, DISPATCH_TIME_FOREVER);
}

void writer_exit(shared_resource_t *resource) {
    dispatch_semaphore_signal(resource->write_semaphore);
}

int read_data(shared_resource_t *resource) {
    reader_entry(resource);
    
    THREAD_SAFE_PRINT("[READER] Reading data: %d (active readers: %d)\n", 
                      resource->data, resource->read_count);
    
    sleep(1);
    
    int data = resource->data;
    
    reader_exit(resource);
    
    return data;
}

void write_data(shared_resource_t *resource, int value) {
    writer_entry(resource);
    
    THREAD_SAFE_PRINT("[WRITER] Writing data: %d\n", value);
    
    sleep(1);
    
    resource->data = value;
    
    THREAD_SAFE_PRINT("[WRITER] Write complete. New value: %d\n", value);
    
    writer_exit(resource);
}

void shared_resource_destroy(shared_resource_t *resource) {
    pthread_mutex_destroy(&resource->read_count_mutex);
}
