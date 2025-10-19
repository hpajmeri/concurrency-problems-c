#ifndef BARBERSHOP_H
#define BARBERSHOP_H

#include <dispatch/dispatch.h>


typedef struct {
    int num_barbers;
    int num_chairs;
    int customers_waiting;
    dispatch_semaphore_t barberSem;
    dispatch_semaphore_t customerSem;
    dispatch_semaphore_t accessSem;
} barbershop_t;

void barbershop_init(barbershop_t *shop, int num_barbers, int num_chairs);

int customer_enter(barbershop_t *shop);

void barber_wait_for_customer(barbershop_t *shop);

void barber_cut_hair(barbershop_t *shop, int barber_id);

void customer_get_haircut(barbershop_t *shop, int customer_id);

void barbershop_destroy(barbershop_t *shop);

#endif /* BARBERSHOP_H */
