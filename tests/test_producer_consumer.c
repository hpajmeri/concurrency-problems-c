#include "common.h"
#include "producer_consumer.h"

typedef struct {
    bounded_buffer_t *bb;
    int thread_id;
} producer_args_t;

typedef struct {
    bounded_buffer_t *bb;
    int thread_id;
} consumer_args_t;

void *producer_thread(void *arg) {
    producer_args_t *args = (producer_args_t *)arg;
    
    for (int i = 0; i < NUM_ITEMS / 2; i++) {
        int item = args->thread_id * 100 + i;
        produce_item(args->bb, item);
        sleep(1);
    }
    
    free(args);
    return NULL;
}

void *consumer_thread(void *arg) {
    consumer_args_t *args = (consumer_args_t *)arg;
    
    for (int i = 0; i < NUM_ITEMS / 2; i++) {
        int item __attribute__((unused)) = consume_item(args->bb);
        sleep(1);
    }
    
    free(args);
    return NULL;
}

int main() {
    bounded_buffer_t bb;
    bounded_buffer_init(&bb);
    
    pthread_t producers[2];
    pthread_t consumers[2];
    
    THREAD_SAFE_PRINT("=== Producer-Consumer Problem ===\n");
    THREAD_SAFE_PRINT("Creating 2 producers and 2 consumers...\n\n");
    
    /* Create producer threads */
    for (int i = 0; i < 2; i++) {
        producer_args_t *args = malloc(sizeof(producer_args_t));
        args->bb = &bb;
        args->thread_id = i + 1;
        pthread_create(&producers[i], NULL, producer_thread, args);
    }
    
    /* Create consumer threads */
    for (int i = 0; i < 2; i++) {
        consumer_args_t *args = malloc(sizeof(consumer_args_t));
        args->bb = &bb;
        args->thread_id = i + 1;
        pthread_create(&consumers[i], NULL, consumer_thread, args);
    }
    
    /* Wait for all threads to finish */
    for (int i = 0; i < 2; i++) {
        pthread_join(producers[i], NULL);
        pthread_join(consumers[i], NULL);
    }
    
    THREAD_SAFE_PRINT("\nAll threads completed.\n");
    
    bounded_buffer_destroy(&bb);
    
    return EXIT_SUCCESS;
}
