#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to get the CPU load using the /proc/loadavg file
void get_cpu_load(char *cpu_load) {
    FILE *fp = fopen("/proc/loadavg", "r");
    if (fp == NULL) {
        perror("Error reading /proc/loadavg");
        strcpy(cpu_load, "N/A");
        return;
    }
    fgets(cpu_load, BUFFER_SIZE, fp);
    fclose(fp);
}

// Function to get the current date and time
void get_current_datetime(char *datetime) {
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(datetime, BUFFER_SIZE, "%Y-%m-%d %H:%M:%S", timeinfo);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    char datetime[BUFFER_SIZE];
    char cpu_load[BUFFER_SIZE];

    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Setup server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  // Listen on all interfaces
    server_addr.sin_port = htons(PORT);  // Listen on port 8080

    // Bind the socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 10) == -1) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept client connections
    while ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) > 0) {
        printf("New connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Get current date-time and CPU load
        get_current_datetime(datetime);
        get_cpu_load(cpu_load);

        // Prepare the message to send
        snprintf(buffer, sizeof(buffer), "Date-Time: %s\nCPU Load: %s", datetime, cpu_load);

        // Send the date-time and CPU load information to the client
        send(client_fd, buffer, strlen(buffer), 0);

        // Close the connection with the client
        close(client_fd);
    }

    if (client_fd < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Close the server socket
    close(server_fd);
    return 0;
}

