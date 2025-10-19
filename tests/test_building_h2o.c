#include "common.h"
#include "building_h2o.h"

typedef struct {
    h2o_t *h2o;
    int id;
} h_args_t;

typedef struct {
    h2o_t *h2o;
    int id;
} o_args_t;

void *hydrogen_thread(void *arg) {
    h_args_t *args = (h_args_t *)arg;
    hydrogen_atom(args->h2o, args->id);
    free(args);
    return NULL;
}

void *oxygen_thread(void *arg) {
    o_args_t *args = (o_args_t *)arg;
    oxygen_atom(args->h2o, args->id);
    free(args);
    return NULL;
}

int main() {
    h2o_t h2o;
    h2o_init(&h2o);
    
    /* Create 6 hydrogen atoms and 3 oxygen atoms (forms 3 H2O molecules) */
    pthread_t h_threads[6];
    pthread_t o_threads[3];
    
    THREAD_SAFE_PRINT("=== Building H2O Problem ===\n");
    THREAD_SAFE_PRINT("Assembling 3 H2O molecules from 6 H atoms and 3 O atoms\n\n");
    
    /* Create oxygen threads first */
    for (int i = 0; i < 3; i++) {
        o_args_t *args = malloc(sizeof(o_args_t));
        args->h2o = &h2o;
        args->id = i + 1;
        pthread_create(&o_threads[i], NULL, oxygen_thread, args);
    }
    
    /* Create hydrogen threads */
    for (int i = 0; i < 6; i++) {
        h_args_t *args = malloc(sizeof(h_args_t));
        args->h2o = &h2o;
        args->id = i + 1;
        pthread_create(&h_threads[i], NULL, hydrogen_thread, args);
    }
    
    /* Wait for all threads */
    for (int i = 0; i < 6; i++) {
        pthread_join(h_threads[i], NULL);
    }
    
    for (int i = 0; i < 3; i++) {
        pthread_join(o_threads[i], NULL);
    }
    
    THREAD_SAFE_PRINT("\nH2O molecules formed: %d\n", h2o.molecules_formed);
    
    h2o_destroy(&h2o);
    return EXIT_SUCCESS;
}
