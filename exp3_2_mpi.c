#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 100  // Size of the array
#define MASTER 0  // Root process ID

// Function to compute the sum of an array
int compute_sum(int *arr, int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    return sum;
}

int main(int argc, char *argv[]) {
    int rank, size;
    int array[N], local_sum = 0, total_sum = 0;

    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Get the rank of the current process
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // Get the total number of processes

    // Initialize the array only in the master process
    if (rank == MASTER) {
        // Populate the array with values (just an example)
        for (int i = 0; i < N; i++) {
            array[i] = i + 1;  // Array: 1, 2, 3, ..., N
        }
    }

    // Scatter the array to all processes
    int local_N = N / size;  // Number of elements each process gets
    int *local_array = (int*)malloc(local_N * sizeof(int));

    MPI_Scatter(array, local_N, MPI_INT, local_array, local_N, MPI_INT, MASTER, MPI_COMM_WORLD);

    // Each process computes the sum of its portion of the array
    local_sum = compute_sum(local_array, local_N);

    // Gather all the local sums at the root process
    MPI_Gather(&local_sum, 1, MPI_INT, &total_sum, 1, MPI_INT, MASTER, MPI_COMM_WORLD);

    // Only the root process will compute the final sum
    if (rank == MASTER) {
        for (int i = 0; i < size; i++) {
            total_sum += total_sum;  // Add each process's local sum
        }

        printf("Total sum: %d\n", total_sum);
    }

    // Finalize MPI
    MPI_Finalize();

    return 0;
}
