#include "common.h"
#include "barbershop.h"

typedef struct {
    barbershop_t *shop;
    int customer_id;
} customer_args_t;

typedef struct {
    barbershop_t *shop;
    int barber_id;
} barber_args_t;

void *barber_thread(void *arg) {
    barber_args_t *args = (barber_args_t *)arg;
    
    THREAD_SAFE_PRINT("[BARBER %d] Starting shift\n", args->barber_id);
    
    for (int i = 0; i < 3; i++) {
        barber_wait_for_customer(args->shop);
        barber_cut_hair(args->shop, args->barber_id);
    }
    
    THREAD_SAFE_PRINT("[BARBER %d] End of shift\n", args->barber_id);
    free(args);
    return NULL;
}

void *customer_thread(void *arg) {
    customer_args_t *args = (customer_args_t *)arg;
    
    if (customer_enter(args->shop)) {
        customer_get_haircut(args->shop, args->customer_id);
        THREAD_SAFE_PRINT("[CUSTOMER %d] Leaving barbershop\n", args->customer_id);
    }
    
    free(args);
    return NULL;
}

int main() {
    barbershop_t shop;
    barbershop_init(&shop, 2, 3);  /* 2 barbers, 3 waiting chairs */
    
    pthread_t barbers[2];
    pthread_t customers[8];
    
    THREAD_SAFE_PRINT("=== Barbershop Problem ===\n");
    THREAD_SAFE_PRINT("2 barbers, 3 waiting chairs\n\n");
    
    /* Create barber threads */
    for (int i = 0; i < 2; i++) {
        barber_args_t *args = malloc(sizeof(barber_args_t));
        args->shop = &shop;
        args->barber_id = i + 1;
        pthread_create(&barbers[i], NULL, barber_thread, args);
    }
    
    /* Create customer threads */
    for (int i = 0; i < 8; i++) {
        customer_args_t *args = malloc(sizeof(customer_args_t));
        args->shop = &shop;
        args->customer_id = i + 1;
        pthread_create(&customers[i], NULL, customer_thread, args);
        sleep(1);
    }
    
    /* Wait for all threads */
    for (int i = 0; i < 2; i++) {
        pthread_join(barbers[i], NULL);
    }
    
    for (int i = 0; i < 8; i++) {
        pthread_join(customers[i], NULL);
    }
    
    THREAD_SAFE_PRINT("\nBarbershop closed for the day!\n");
    
    barbershop_destroy(&shop);
    return EXIT_SUCCESS;
}
