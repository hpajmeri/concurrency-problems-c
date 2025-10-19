#ifndef COMMON_H
#define COMMON_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_BUFFER_SIZE 10
#define MAX_THREADS 20
#define NUM_ITEMS 50
extern pthread_mutex_t print_mutex;

#define THREAD_SAFE_PRINT(fmt, ...) \
    do { \
        pthread_mutex_lock(&print_mutex); \
        printf(fmt, ##__VA_ARGS__); \
        fflush(stdout); \
        pthread_mutex_unlock(&print_mutex); \
    } while(0)

#endif /* COMMON_H */
