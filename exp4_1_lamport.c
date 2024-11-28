#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_PROCESSES 3

typedef struct {
    int id;
    int logical_clock;
} Process;

// Function to simulate the sending of a message
void send_message(Process *sender, Process *receiver) {
    // Sender sends message with its clock
    printf("Process %d sending message with logical clock %d\n", sender->id, sender->logical_clock);
    
    // Receiver updates its clock
    if (receiver->logical_clock < sender->logical_clock) {
        receiver->logical_clock = sender->logical_clock;
    }
    receiver->logical_clock++;  // Receiver increments its clock after receiving
    printf("Process %d received message and updated its clock to %d\n", receiver->id, receiver->logical_clock);
}

int main() {
    Process processes[NUM_PROCESSES];
    
    // Initialize processes with IDs and clocks
    for (int i = 0; i < NUM_PROCESSES; i++) {
        processes[i].id = i;
        processes[i].logical_clock = 0;
    }

    // Simulate events: Process 0 sends a message to Process 1, and Process 1 sends a message to Process 2
    processes[0].logical_clock++;  // Process 0 increments its clock for the first event
    send_message(&processes[0], &processes[1]);

    processes[1].logical_clock++;  // Process 1 increments its clock for the second event
    send_message(&processes[1], &processes[2]);

    // Final clock values
    printf("\nFinal Clock Values:\n");
    for (int i = 0; i < NUM_PROCESSES; i++) {
        printf("Process %d: %d\n", processes[i].id, processes[i].logical_clock);
    }

    return 0;
}

