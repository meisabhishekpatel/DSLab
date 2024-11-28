#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define SHM_NAME "/counter_shm"  // Shared memory object name
#define SEM_NAME "/counter_sem"  // Semaphore name

int main() {
    // Define the size of the shared memory (counter)
    const size_t shm_size = sizeof(int);

    // Open the shared memory object (shared memory already created by server)
    int shm_fd = shm_open(SHM_NAME, O_RDWR, S_IRUSR | S_IWUSR);
    if (shm_fd == -1) {
        perror("Failed to open shared memory");
        exit(1);
    }

    // Map the shared memory object into the address space of the client
    int *counter = mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (counter == MAP_FAILED) {
        perror("Failed to map shared memory");
        exit(1);
    }

    // Open the semaphore (already created by the server)
    sem_t *sem = sem_open(SEM_NAME, 0);
    if (sem == SEM_FAILED) {
        perror("Failed to open semaphore");
        exit(1);
    }

    // Client loop: read the counter value from shared memory
    for (int i = 0; i < 10; i++) {
        // Wait (decrement) on the semaphore to enter the critical section
        sem_wait(sem);

        // Read and print the counter value from shared memory
        printf("Current counter value: %d\n", *counter);

        // Signal (increment) the semaphore to release the critical section
        sem_post(sem);

        // Sleep for a short time to simulate periodic reading
        sleep(1);
    }

    // Cleanup
    munmap(counter, shm_size); // Unmap the shared memory
    close(shm_fd);             // Close the shared memory object
    sem_close(sem);            // Close the semaphore

    return 0;
}

