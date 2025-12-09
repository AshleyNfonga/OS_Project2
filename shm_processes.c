#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <time.h>

// Shared memory structure
typedef struct {
    int BankAccount;
    sem_t mutex;   // semaphore for mutual exclusion
} SharedData;

// Dear Old Dad process
void dad_process(SharedData *shared) {
    int localBalance = 0;
    srand(time(NULL) ^ getpid());

    while (1) {
        sleep(rand() % 6); // 0-5 seconds
        printf("Dear Old Dad: Attempting to Check Balance\n");
        int randNum = rand() % 2;

        if (randNum == 0) { // deposit attempt
            sem_wait(&shared->mutex);
            localBalance = shared->BankAccount;

            if (localBalance < 100) {
                int amount = rand() % 101; // 0-100
                shared->BankAccount += amount;
                localBalance = shared->BankAccount;
                printf("Dear Old Dad: Deposits $%d / Balance = $%d\n", amount, localBalance);
            } else {
                printf("Dear Old Dad: Thinks Student has enough Cash ($%d)\n", localBalance);
            }
            sem_post(&shared->mutex);
        } else { // just check balance
            sem_wait(&shared->mutex);
            localBalance = shared->BankAccount;
            printf("Dear Old Dad: Last Checking Balance = $%d\n", localBalance);
            sem_post(&shared->mutex);
        }
    }
}

// Lovable Mom process (Extra Credit)
void mom_process(SharedData *shared) {
    int localBalance = 0;
    srand(time(NULL) ^ getpid());

    while (1) {
        sleep(rand() % 11); // 0-10 seconds
        printf("Lovable Mom: Attempting to Check Balance\n");

        sem_wait(&shared->mutex);
        localBalance = shared->BankAccount;

        if (localBalance <= 100) {
            int amount = rand() % 126; // 0-125
            shared->BankAccount += amount;
            localBalance = shared->BankAccount;
            printf("Lovable Mom: Deposits $%d / Balance = $%d\n", amount, localBalance);
        }
        sem_post(&shared->mutex);
    }
}

// Poor Student process
void student_process(SharedData *shared) {
    int localBalance = 0;
    srand(time(NULL) ^ getpid());

    while (1) {
        sleep(rand() % 6); // 0-5 seconds
        printf("Poor Student: Attempting to Check Balance\n");
        int randNum = rand() % 2;

        sem_wait(&shared->mutex);
        localBalance = shared->BankAccount;

        if (randNum == 0) { // withdraw attempt
            int need = rand() % 51; // 0-50
            printf("Poor Student needs $%d\n", need);
            if (need <= localBalance) {
                shared->BankAccount -= need;
                localBalance = shared->BankAccount;
                printf("Poor Student: Withdraws $%d / Balance = $%d\n", need, localBalance);
            } else {
                printf("Poor Student: Not Enough Cash ($%d)\n", localBalance);
            }
        } else { // just check balance
            printf("Poor Student: Last Checking Balance = $%d\n", localBalance);
        }

        sem_post(&shared->mutex);
    }
}

// Main function
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <num_parents> <num_students>\n", argv[0]);
        printf("num_parents = 1 (Dad) or 2 (Dad + Mom)\n");
        return 1;
    }

    int num_parents = atoi(argv[1]);
    int num_students = atoi(argv[2]);

    // Create shared memory
    SharedData *shared = mmap(NULL, sizeof(SharedData),
                              PROT_READ | PROT_WRITE,
                              MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shared == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    shared->BankAccount = 0;
    sem_init(&shared->mutex, 1, 1); // shared semaphore

    // Fork parents
    if (num_parents >= 1) {
        if (fork() == 0) { dad_process(shared); exit(0); }
    }
    if (num_parents == 2) {
        if (fork() == 0) { mom_process(shared); exit(0); }
    }

    // Fork students
    for (int i = 0; i < num_students; i++) {
        if (fork() == 0) { student_process(shared); exit(0); }
    }

    // Wait for all child processes
    for (int i = 0; i < num_parents + num_students; i++) {
        wait(NULL);
    }

    return 0;
}