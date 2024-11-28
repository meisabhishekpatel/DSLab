#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>

#define SHM_NAME "/counter_shm"
#define SEM_NAME "/counter_sem"

int main() {
    // Define the size of the shared memory (counter)
    const size_t shm_size = sizeof(int);

    // Open the shared memory object
    int shm_fd = shm_open(SHM_NAME, O_RDWR, S_IRUSR | S_IWUSR);
    if (shm_fd == -1) {
        perror("Failed to open shared memory");
        exit(1);
    }

    // Map the shared memory object into the address space
    int *counter = mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (counter == MAP_FAILED) {
        perror("Failed to map shared memory");
        exit(1);
    }

    // Open the semaphore
    sem_t *sem = sem_open(SEM_NAME, 0);
    if (sem == SEM_FAILED) {
        perror("Failed to open semaphore");
        exit(1);
    }

    // Client loop: read and print the counter value
    for (int i = 0; i < 10; i++) {
        // Wait for the semaphore (critical section)
        sem_wait(sem);

        // Read and print the counter value
        printf("Current counter value: %d\n", *counter);

        // Release the semaphore (leave critical section)
        sem_post(sem);

        sleep(1); // Sleep before reading again
    }

    // Cleanup
    munmap(counter, shm_size); // Unmap the shared memory
    close(shm_fd);             // Close the shared memory object
    sem_close(sem);            // Close the semaphore

    return 0;
}
