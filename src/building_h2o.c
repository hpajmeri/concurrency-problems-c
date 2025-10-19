#include "common.h"
#include "building_h2o.h"
#include <pthread.h>

void h2o_init(h2o_t *h2o) {
    h2o->h_count = 0;
    h2o->o_count = 0;
    h2o->molecules_formed = 0;
    
    h2o->h_sem = dispatch_semaphore_create(0);
    h2o->o_sem = dispatch_semaphore_create(0);
    h2o->barrier = dispatch_semaphore_create(0);

    h2o->count_lock = dispatch_semaphore_create(1);
}

void hydrogen_atom(h2o_t *h2o, int id) {
    THREAD_SAFE_PRINT("[H %d] Ready to bond\n", id);
    
    dispatch_semaphore_wait(h2o->count_lock, DISPATCH_TIME_FOREVER);
    h2o->h_count++;

    THREAD_SAFE_PRINT("[H %d] Waiting (H: %d, O: %d)\n", id, h2o->h_count, h2o->o_count);

    if (h2o->h_count >= 2 && h2o->o_count >= 1) {
        THREAD_SAFE_PRINT("[H %d] 2 hydrogens ready! Signaling oxygen...\n", id);
        dispatch_semaphore_signal(h2o->o_sem);
    }

    dispatch_semaphore_signal(h2o->count_lock);

    dispatch_semaphore_wait(h2o->h_sem, DISPATCH_TIME_FOREVER);

    THREAD_SAFE_PRINT("[H %d] Bonded! H2O molecule #%d formed!\n", id, h2o->molecules_formed);
}

void oxygen_atom(h2o_t *h2o, int id) {
    THREAD_SAFE_PRINT("[O %d] Ready to bond\n", id);
    
    dispatch_semaphore_wait(h2o->count_lock, DISPATCH_TIME_FOREVER);
    h2o->o_count++;

    THREAD_SAFE_PRINT("[O %d] Waiting (H: %d, O: %d)\n", id, h2o->h_count, h2o->o_count);

    if (h2o->h_count < 2) {
        dispatch_semaphore_signal(h2o->count_lock);
        dispatch_semaphore_wait(h2o->o_sem, DISPATCH_TIME_FOREVER);
        dispatch_semaphore_wait(h2o->count_lock, DISPATCH_TIME_FOREVER);
    }

    if (h2o->h_count >= 2 && h2o->o_count >= 1) {
        THREAD_SAFE_PRINT("[O %d] Ready! Forming H2O molecule #%d\n", id, h2o->molecules_formed + 1);
        h2o->molecules_formed++;

        /* Consume hydrogens */
        h2o->h_count -= 2;
        h2o->o_count -= 1;

        dispatch_semaphore_signal(h2o->h_sem);
        dispatch_semaphore_signal(h2o->h_sem);
    }

    dispatch_semaphore_signal(h2o->count_lock);

    sleep(1);
}

void h2o_destroy(h2o_t *h2o __attribute__((unused))) {
    
}
