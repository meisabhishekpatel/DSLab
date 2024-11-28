#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define NUM_PROCESSES 3
#define CRITICAL_SECTION_TIME 2

// gcc exp4_2_mutex_both.c -lrt -pthread

// Struct to represent a process
typedef struct {
    int id;
    int logical_clock;
    int request_time;
    int is_in_cs;
    pthread_mutex_t cs_mutex;
} Process;

// Message structure for requests
typedef struct {
    int process_id;
    int timestamp;
} Message;

Process processes[NUM_PROCESSES];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond[NUM_PROCESSES];

void enter_critical_section(int id) {
    printf("Process %d entering critical section\n", id);
    sleep(CRITICAL_SECTION_TIME);  // Simulate time spent in critical section
    printf("Process %d leaving critical section\n", id);
}

// Function to simulate Lamport's Algorithm for mutual exclusion
void* lamport_algorithm(void* arg) {
    int id = *((int*)arg);
    printf("Process %d started\n", id);

    // Each process wants to enter the critical section
    processes[id].logical_clock++;
    processes[id].request_time = processes[id].logical_clock;

    // Send request messages to all other processes
    for (int i = 0; i < NUM_PROCESSES; i++) {
        if (i != id) {
            pthread_mutex_lock(&lock);
            printf("Process %d sending request with timestamp %d to Process %d\n", id, processes[id].request_time, i);
            pthread_cond_signal(&cond[i]);
            pthread_mutex_unlock(&lock);
        }
    }

    // Wait for all replies before entering critical section
    for (int i = 0; i < NUM_PROCESSES - 1; i++) {
        pthread_mutex_lock(&lock);
        pthread_cond_wait(&cond[id], &lock);  // Wait for reply
        pthread_mutex_unlock(&lock);
    }

    // Enter critical section
    enter_critical_section(id);

    // Update clock after critical section
    processes[id].logical_clock++;

    return NULL;
}

// Function to simulate Ricart-Agrawala's Algorithm for mutual exclusion
void* ricart_agrawala_algorithm(void* arg) {
    int id = *((int*)arg);
    printf("Process %d started\n", id);

    // Each process wants to enter the critical section
    processes[id].request_time = processes[id].logical_clock + 1;

    // Send request messages to all other processes
    for (int i = 0; i < NUM_PROCESSES; i++) {
        if (i != id) {
            pthread_mutex_lock(&lock);
            printf("Process %d sending request with timestamp %d to Process %d\n", id, processes[id].request_time, i);
            pthread_cond_signal(&cond[i]);
            pthread_mutex_unlock(&lock);
        }
    }

    // Wait for all replies before entering critical section
    for (int i = 0; i < NUM_PROCESSES - 1; i++) {
        pthread_mutex_lock(&lock);
        pthread_cond_wait(&cond[id], &lock);  // Wait for reply
        pthread_mutex_unlock(&lock);
    }

    // Enter critical section
    enter_critical_section(id);

    // Update clock after critical section
    processes[id].logical_clock++;

    return NULL;
}

int main() {
    pthread_t threads[NUM_PROCESSES];
    int ids[NUM_PROCESSES];

    // Initialize processes
    for (int i = 0; i < NUM_PROCESSES; i++) {
        ids[i] = i;
        processes[i].id = i;
        processes[i].logical_clock = 0;
        processes[i].is_in_cs = 0;
        pthread_mutex_init(&processes[i].cs_mutex, NULL);
        pthread_cond_init(&cond[i], NULL);
    }

    // Choose which algorithm to run
    char choice;
    printf("Select Mutual Exclusion Algorithm (L for Lamport, R for Ricart-Agrawala): ");
    scanf(" %c", &choice);

    if (choice == 'L') {
        // Simulate Lamport’s Algorithm
        for (int i = 0; i < NUM_PROCESSES; i++) {
            pthread_create(&threads[i], NULL, lamport_algorithm, (void*)&ids[i]);
        }
    } else if (choice == 'R') {
        // Simulate Ricart-Agrawala Algorithm
        for (int i = 0; i < NUM_PROCESSES; i++) {
            pthread_create(&threads[i], NULL, ricart_agrawala_algorithm, (void*)&ids[i]);
        }
    } else {
        printf("Invalid choice!\n");
        return 1;
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_PROCESSES; i++) {
        pthread_join(threads[i], NULL);
    }

    // Clean up
    for (int i = 0; i < NUM_PROCESSES; i++) {
        pthread_mutex_destroy(&processes[i].cs_mutex);
        pthread_cond_destroy(&cond[i]);
    }

    return 0;
}

