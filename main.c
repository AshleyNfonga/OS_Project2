#include "BENSCHILLIBOWL.h"
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_QUEUE_SIZE 10
#define NUM_CUSTOMERS 5
#define NUM_COOKS 2

BENSCHILLIBOWL* bcb;  // global restaurant

void* BENSCHILLIBOWLCustomer(void* tid) {
    int id = *((int*)tid);
    free(tid);

    Order *order = malloc(sizeof(Order));
    if (!order) {
        perror("Failed to allocate order");
        return NULL;
    }

    order->customer_id = id;
    order->item = PickRandomMenuItem();

    printf("Customer %d: Placing order %d\n", id, order->item);
    AddOrder(bcb, order);
    return NULL;
}

void* BENSCHILLIBOWLCook(void* tid) {
    int id = *((int*)tid);
    free(tid);

    while (true) {
        Order *order = GetOrder(bcb);
        if (order == NULL) break;  // Poison pill signals exit

        printf("Cook %d: Fulfilling order from Customer %d for item %d\n",
               id, order->customer_id, order->item);
        free(order);
        sleep(1);  // simulate cooking time
    }
    printf("Cook %d: No more orders, exiting.\n", id);
    return NULL;
}

int main() {
    bcb = OpenRestaurant(MAX_QUEUE_SIZE, NUM_CUSTOMERS);

    pthread_t customers[NUM_CUSTOMERS];
    pthread_t cooks[NUM_COOKS];

    // Create customer threads
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        int *id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&customers[i], NULL, BENSCHILLIBOWLCustomer, id);
    }

    // Create cook threads
    for (int i = 0; i < NUM_COOKS; i++) {
        int *id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&cooks[i], NULL, BENSCHILLIBOWLCook, id);
    }

    // Wait for all customers to finish
    for (int i = 0; i < NUM_CUSTOMERS; i++)
        pthread_join(customers[i], NULL);

    // Send poison pills (NULL orders) to signal cooks to exit
    for (int i = 0; i < NUM_COOKS; i++) {
        AddOrder(bcb, NULL);
    }

    // Wait for all cooks to finish
    for (int i = 0; i < NUM_COOKS; i++)
        pthread_join(cooks[i], NULL);

    CloseRestaurant(bcb);
    return 0;
}
