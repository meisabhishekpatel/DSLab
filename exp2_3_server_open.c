#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define SHM_NAME "/counter_shm"  // Shared memory object name
#define SEM_NAME "/counter_sem"  // Semaphore name

// gcc -o server server.c -lrt -pthread
// gcc -o client client.c -lrt -pthread

int main() {
    // Define the size of the shared memory (counter)
    const size_t shm_size = sizeof(int);

    // Create or open the shared memory object
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shm_fd == -1) {
        perror("Failed to create/open shared memory");
        exit(1);
    }

    // Set the size of the shared memory object
    if (ftruncate(shm_fd, shm_size) == -1) {
        perror("Failed to set size of shared memory");
        exit(1);
    }

    // Map the shared memory object into the address space of the server
    int *counter = mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (counter == MAP_FAILED) {
        perror("Failed to map shared memory");
        exit(1);
    }

    // Initialize the counter in shared memory to 0
    *counter = 0;

    // Create the semaphore (if it doesn't exist) and initialize it
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, S_IRUSR | S_IWUSR, 1);
    if (sem == SEM_FAILED) {
        perror("Failed to create semaphore");
        exit(1);
    }

    // Server loop: increment the counter in shared memory
    for (int i = 0; i < 10; i++) {
        // Wait (decrement) the semaphore to enter the critical section
        sem_wait(sem);

        // Increment the counter
        (*counter)++;
        printf("Server incremented counter to: %d\n", *counter);

        // Signal (increment) the semaphore to release the critical section
        sem_post(sem);

        // Sleep for a short time to simulate periodic incrementing
        sleep(1);
    }

    // Cleanup
    munmap(counter, shm_size); // Unmap the shared memory
    close(shm_fd);             // Close the shared memory object
    sem_close(sem);            // Close the semaphore
    sem_unlink(SEM_NAME);      // Unlink (delete) the semaphore
    shm_unlink(SHM_NAME);     // Unlink (delete) the shared memory object

    return 0;
}

