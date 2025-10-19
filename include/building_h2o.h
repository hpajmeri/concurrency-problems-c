#ifndef BUILDING_H2O_H
#define BUILDING_H2O_H

#include <dispatch/dispatch.h>

/* Building H2O Problem
 * Oxygen and Hydrogen atoms combine to form water molecules (H2O)
 * Need 2 Hydrogen atoms and 1 Oxygen atom
 * Synchronization to ensure proper pairing and formation
 */

typedef struct {
    int h_count;
    int o_count;
    int molecules_formed;
    
    /* Semaphores for coordination */
    dispatch_semaphore_t h_sem;         /* Hydrogen atom ready */
    dispatch_semaphore_t o_sem;         /* Oxygen atom ready */
    dispatch_semaphore_t barrier;       /* Ensure synchronized bonding */
    
    /* Mutexes for counters */
    /* Single mutex protecting both counters to avoid inconsistent reads
     * and lock ordering issues. */
    dispatch_semaphore_t count_lock;   /* Protect h_count and o_count */
} h2o_t;

/* Initialize H2O molecule builder */
void h2o_init(h2o_t *h2o);

/* Hydrogen atom waits for bonding */
void hydrogen_atom(h2o_t *h2o, int id);

/* Oxygen atom waits for bonding */
void oxygen_atom(h2o_t *h2o, int id);

/* Cleanup */
void h2o_destroy(h2o_t *h2o);

#endif /* BUILDING_H2O_H */
