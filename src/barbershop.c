#include "common.h"
#include "barbershop.h"

void barbershop_init(barbershop_t *shop, int num_barbers, int num_chairs) {
    shop->num_barbers = num_barbers;
    shop->num_chairs = num_chairs;
    shop->customers_waiting = 0;
    shop->barberSem = dispatch_semaphore_create(0);
    shop->customerSem = dispatch_semaphore_create(0);
    shop->accessSem = dispatch_semaphore_create(1);
}

int customer_enter(barbershop_t *shop) {
    dispatch_semaphore_wait(shop->accessSem, DISPATCH_TIME_FOREVER);
    
    if (shop->customers_waiting >= shop->num_chairs) {
        dispatch_semaphore_signal(shop->accessSem);
        THREAD_SAFE_PRINT("[CUSTOMER] Waiting room full, leaving!\n");
        return 0;  /* Balk - customer leaves */
    }
    
    shop->customers_waiting++;
    THREAD_SAFE_PRINT("[CUSTOMER] Entering barbershop. Waiting: %d\n", shop->customers_waiting);
    
    dispatch_semaphore_signal(shop->customerSem);
    dispatch_semaphore_signal(shop->accessSem);
    
    return 1;  /* Successfully entered */
}

void barber_wait_for_customer(barbershop_t *shop) {
    dispatch_semaphore_wait(shop->customerSem, DISPATCH_TIME_FOREVER);
    dispatch_semaphore_wait(shop->accessSem, DISPATCH_TIME_FOREVER);
    shop->customers_waiting--;
    dispatch_semaphore_signal(shop->accessSem);
}

void barber_cut_hair(barbershop_t *shop __attribute__((unused)), int barber_id) {
    THREAD_SAFE_PRINT("[BARBER %d] Cutting hair...\n", barber_id);
    sleep(2);
    THREAD_SAFE_PRINT("[BARBER %d] Finished haircut!\n", barber_id);
}

void customer_get_haircut(barbershop_t *shop __attribute__((unused)), int customer_id) {
    THREAD_SAFE_PRINT("[CUSTOMER %d] Getting haircut...\n", customer_id);
    sleep(2);
    THREAD_SAFE_PRINT("[CUSTOMER %d] Haircut complete!\n", customer_id);
}

void barbershop_destroy(barbershop_t *shop __attribute__((unused))) {
}
