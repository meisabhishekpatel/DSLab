#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <ctype.h>

#define PORT 9090   // Load balancer listens on this port
#define SERVER1_PORT 8081
#define SERVER2_PORT 8082
#define BUFFER_SIZE 1024

// Function to get CPU load of a server by reading /proc/loadavg
double get_cpu_load(const char *server_ip, int port) {
    // This function will query the server for its CPU load.
    // Here we just return a dummy value for simplicity.
    return rand() % 100 / 100.0;  // Random CPU load between 0 and 1
}

// Function to communicate with a server and send the string
void communicate_with_server(const char *server_ip, int port, const char *input_str, char *output_str) {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Setup server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);  // Server's port
    server_addr.sin_addr.s_addr = inet_addr(server_ip);  // Server's IP address

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Send the input string to the server
    send(sock, input_str, strlen(input_str), 0);

    // Receive the uppercase string from the server
    int len = recv(sock, buffer, sizeof(buffer), 0);
    buffer[len] = '\0';  // Null-terminate the string

    strcpy(output_str, buffer);

    // Close the socket
    close(sock);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    char result_str[BUFFER_SIZE];

    // Create server socket for the load balancer
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Setup load balancer server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);  // Load balancer's port

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

    printf("Load balancer listening on port %d...\n", PORT);

    // Accept client connections
    while ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) > 0) {
        printf("Received connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Receive the string from the client
        int len = recv(client_fd, buffer, sizeof(buffer), 0);
        buffer[len] = '\0';  // Null-terminate the string

        // Get CPU loads for both servers
        double server1_load = get_cpu_load("127.0.0.1", SERVER1_PORT);
        double server2_load = get_cpu_load("127.0.0.1", SERVER2_PORT);

        // Select the server with the lesser CPU load
        if (server1_load < server2_load) {
            printf("Sending to server 1 with CPU load: %f\n", server1_load);
            communicate_with_server("127.0.0.1", SERVER1_PORT, buffer, result_str);
        } else {
            printf("Sending to server 2 with CPU load: %f\n", server2_load);
            communicate_with_server("127.0.0.1", SERVER2_PORT, buffer, result_str);
        }

        // Send the uppercase string back to the client
        send(client_fd, result_str, strlen(result_str), 0);

        // Close the client connection
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

