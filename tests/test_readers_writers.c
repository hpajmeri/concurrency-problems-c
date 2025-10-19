#include "common.h"
#include "readers_writers.h"

typedef struct {
    shared_resource_t *resource;
    int thread_id;
} reader_args_t;

typedef struct {
    shared_resource_t *resource;
    int thread_id;
} writer_args_t;

void *reader_thread(void *arg) {
    reader_args_t *args = (reader_args_t *)arg;
    
    for (int i = 0; i < 3; i++) {
        int value __attribute__((unused)) = read_data(args->resource);
        sleep(1);
    }
    
    free(args);
    return NULL;
}

void *writer_thread(void *arg) {
    writer_args_t *args = (writer_args_t *)arg;
    
    for (int i = 0; i < 3; i++) {
        write_data(args->resource, args->thread_id * 10 + i);
        sleep(2);
    }
    
    free(args);
    return NULL;
}

int main() {
    shared_resource_t resource;
    shared_resource_init(&resource);
    
    pthread_t readers[3];
    pthread_t writers[2];
    
    THREAD_SAFE_PRINT("=== Readers-Writers Problem ===\n");
    THREAD_SAFE_PRINT("Creating 3 readers and 2 writers...\n\n");
    
    /* Create reader threads */
    for (int i = 0; i < 3; i++) {
        reader_args_t *args = malloc(sizeof(reader_args_t));
        args->resource = &resource;
        args->thread_id = i + 1;
        pthread_create(&readers[i], NULL, reader_thread, args);
    }
    
    /* Create writer threads */
    for (int i = 0; i < 2; i++) {
        writer_args_t *args = malloc(sizeof(writer_args_t));
        args->resource = &resource;
        args->thread_id = i + 1;
        pthread_create(&writers[i], NULL, writer_thread, args);
    }
    
    /* Wait for all threads to finish */
    for (int i = 0; i < 3; i++) {
        pthread_join(readers[i], NULL);
    }
    
    for (int i = 0; i < 2; i++) {
        pthread_join(writers[i], NULL);
    }
    
    THREAD_SAFE_PRINT("\nAll threads completed.\n");
    
    shared_resource_destroy(&resource);
    
    return EXIT_SUCCESS;
}
