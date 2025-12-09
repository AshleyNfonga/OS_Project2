#include "BENSCHILLIBOWL.h"

// Pick a random menu item
MenuItem PickRandomMenuItem() {
    return (MenuItem)(rand() % MAX_MENU_ITEMS);
}

// Open restaurant (initialize)
BENSCHILLIBOWL* OpenRestaurant(int max_size, int expected_num_orders) {
    BENSCHILLIBOWL *bcb = malloc(sizeof(BENSCHILLIBOWL));
    bcb->max_size = max_size;
    bcb->num_orders = 0;
    bcb->front = 0;
    bcb->rear = 0;
    bcb->orders = malloc(sizeof(Order*) * max_size);

    pthread_mutex_init(&bcb->mutex, NULL);
    pthread_cond_init(&bcb->cond_not_full, NULL);
    pthread_cond_init(&bcb->cond_not_empty, NULL);

    srand(time(NULL));  // initialize random
    return bcb;
}

// Close restaurant (cleanup)
void CloseRestaurant(BENSCHILLIBOWL* bcb) {
    pthread_mutex_destroy(&bcb->mutex);
    pthread_cond_destroy(&bcb->cond_not_empty);
    pthread_cond_destroy(&bcb->cond_not_full);

    free(bcb->orders);
    free(bcb);
}

// Check if restaurant queue is full
bool IsFull(BENSCHILLIBOWL* bcb) {
    return bcb->num_orders == bcb->max_size;
}

// Add order to restaurant
int AddOrder(BENSCHILLIBOWL* bcb, Order* order) {
    pthread_mutex_lock(&bcb->mutex);

    while (IsFull(bcb)) {
        pthread_cond_wait(&bcb->cond_not_full, &bcb->mutex);
    }

    bcb->orders[bcb->rear] = order;
    bcb->rear = (bcb->rear + 1) % bcb->max_size;
    bcb->num_orders++;

    pthread_cond_signal(&bcb->cond_not_empty);
    pthread_mutex_unlock(&bcb->mutex);
    return 0;
}

// Get order from restaurant
Order* GetOrder(BENSCHILLIBOWL* bcb) {
    pthread_mutex_lock(&bcb->mutex);

    while (bcb->num_orders == 0) {
        pthread_cond_wait(&bcb->cond_not_empty, &bcb->mutex);
    }

    Order *order = bcb->orders[bcb->front];
    bcb->front = (bcb->front + 1) % bcb->max_size;
    bcb->num_orders--;

    pthread_cond_signal(&bcb->cond_not_full);
    pthread_mutex_unlock(&bcb->mutex);
    return order;
}