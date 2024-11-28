#include <stdio.h>
#include <stdlib.h>

#define NUM_PROCESSES 3

typedef struct {
    int id;
    int vector_clock[NUM_PROCESSES];
} Process;

// Function to print the vector clock
void print_vector_clock(int *vector_clock) {
    for (int i = 0; i < NUM_PROCESSES; i++) {
        printf("%d ", vector_clock[i]);
    }
    printf("\n");
}

// Function to simulate sending a message with the vector clock
void send_message(Process *sender, Process *receiver) {
    // Sender sends its vector clock
    printf("Process %d sending message with vector clock: ", sender->id);
    print_vector_clock(sender->vector_clock);

    // Receiver updates its vector clock
    for (int i = 0; i < NUM_PROCESSES; i++) {
        if (receiver->vector_clock[i] < sender->vector_clock[i]) {
            receiver->vector_clock[i] = sender->vector_clock[i];
        }
    }
    receiver->vector_clock[receiver->id]++;  // Increment the receiver's own clock
    printf("Process %d received message and updated its vector clock: ", receiver->id);
    print_vector_clock(receiver->vector_clock);
}

int main() {
    Process processes[NUM_PROCESSES];
    
    // Initialize processes with IDs and vector clocks
    for (int i = 0; i < NUM_PROCESSES; i++) {
        processes[i].id = i;
        for (int j = 0; j < NUM_PROCESSES; j++) {
            processes[i].vector_clock[j] = 0;
        }
    }

    // Simulate events: Process 0 sends a message to Process 1, and Process 1 sends a message to Process 2
    processes[0].vector_clock[0]++;  // Process 0 performs an event
    send_message(&processes[0], &processes[1]);

    processes[1].vector_clock[1]++;  // Process 1 performs an event
    send_message(&processes[1], &processes[2]);

    // Final vector clock values
    printf("\nFinal Vector Clock Values:\n");
    for (int i = 0; i < NUM_PROCESSES; i++) {
        printf("Process %d: ", processes[i].id);
        print_vector_clock(processes[i].vector_clock);
    }

    return 0;
}

