#ifndef BENSCHILLIBOWL_H
#define BENSCHILLIBOWL_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>

#define MAX_MENU_ITEMS 5

typedef enum {
    BURGER,
    TACO,
    CHILI,
    SODA,
    FRIES
} MenuItem;

typedef struct Order {
    int customer_id;
    MenuItem item;
} Order;

typedef struct BENSCHILLIBOWL {
    Order **orders;         // array of order pointers
    int max_size;           // max queue size
    int num_orders;         // current queue size
    int front;              // front index of queue
    int rear;               // rear index of queue
    pthread_mutex_t mutex;
    pthread_cond_t cond_not_full;
    pthread_cond_t cond_not_empty;
} BENSCHILLIBOWL;

// Function declarations
BENSCHILLIBOWL* OpenRestaurant(int max_size, int expected_num_orders);
void CloseRestaurant(BENSCHILLIBOWL* bcb);
int AddOrder(BENSCHILLIBOWL* bcb, Order* order);
Order* GetOrder(BENSCHILLIBOWL* bcb);
bool IsFull(BENSCHILLIBOWL* bcb);
MenuItem PickRandomMenuItem();

#endif