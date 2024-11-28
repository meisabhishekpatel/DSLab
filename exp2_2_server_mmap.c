#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>

// gcc -o server server.c -lrt -pthread
// gcc -o client client.c -lrt -pthread

// Define the name for shared memory and semaphore
#define SHM_NAME "/counter_shm"
#define SEM_NAME "/counter_sem"

int main() {
    // Define the shared memory size (just one integer for the counter)
    const size_t shm_size = sizeof(int);
    
    // Open the shared memory object
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shm_fd == -1) {
        perror("Failed to open shared memory");
        exit(1);
    }

    // Set the size of the shared memory object
    if (ftruncate(shm_fd, shm_size) == -1) {
        perror("Failed to set the size of shared memory");
        exit(1);
    }

    // Map the shared memory object into the address space of the process
    int *counter = mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (counter == MAP_FAILED) {
        perror("Failed to map shared memory");
        exit(1);
    }

    // Initialize the counter in shared memory to 0 if not already set
    *counter = 0;

    // Create or open the semaphore
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, S_IRUSR | S_IWUSR, 1);
    if (sem == SEM_FAILED) {
        perror("Failed to create/open semaphore");
        exit(1);
    }

    // Server loop: increment the counter in the shared memory
    while (1) {
        // Wait (decrement) on the semaphore to enter the critical section
        sem_wait(sem);

        // Increment the counter
        (*counter)++;
        printf("Counter value: %d\n", *counter);

        // Signal (increment) the semaphore to release the critical section
        sem_post(sem);

        // Sleep for a short time to simulate work
        sleep(1);
    }

    // Cleanup
    munmap(counter, shm_size); // Unmap the shared memory
    close(shm_fd);             // Close the shared memory object
    sem_close(sem);            // Close the semaphore

    return 0;
}
