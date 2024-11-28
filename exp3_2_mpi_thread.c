#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define N 100  // Size of the array
#define THREADS 4  // Number of threads

// gcc exp3_2_mpi_thread.c -pthread

int array[N];

typedef struct {
    int start;
    int end;
    int result;
} ThreadData;

// Function to compute the sum of a portion of the array
void* compute_sum(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    data->result = 0;
    for (int i = data->start; i < data->end; i++) {
        data->result += array[i];
    }
    return NULL;
}

int main() {
    // Initialize the array with some values
    for (int i = 0; i < N; i++) {
        array[i] = i + 1;  // Array: 1, 2, 3, ..., N
    }

    pthread_t threads[THREADS];
    ThreadData thread_data[THREADS];
    int segment_size = N / THREADS;

    // Create threads
    for (int i = 0; i < THREADS; i++) {
        thread_data[i].start = i * segment_size;
        thread_data[i].end = (i + 1) * segment_size;
        if (i == THREADS - 1) {
            thread_data[i].end = N;  // Handle the last segment
        }
        pthread_create(&threads[i], NULL, compute_sum, &thread_data[i]);
    }

    // Wait for all threads to finish
    int total_sum = 0;
    for (int i = 0; i < THREADS; i++) {
        pthread_join(threads[i], NULL);
        total_sum += thread_data[i].result;
    }

    printf("Total sum: %d\n", total_sum);
    return 0;
}

