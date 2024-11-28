#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_NODES 10
#define BUFFER_SIZE 1024

// Structure to hold client information (Node Number, IP Address, Port Number)
typedef struct {
    int node_number;
    char ip_address[INET_ADDRSTRLEN];
    int port;
} NodeInfo;

// Global Master Table
NodeInfo master_table[MAX_NODES];
int node_count = 0;  // Keep track of the number of nodes

// Function to update the master table with the new client information
void update_master_table(int node_number, const char *ip_address, int port) {
    if (node_count < MAX_NODES) {
        master_table[node_count].node_number = node_number;
        strcpy(master_table[node_count].ip_address, ip_address);
        master_table[node_count].port = port;
        node_count++;
    } else {
        printf("Master table is full. Cannot add more nodes.\n");
    }
}

// Function to send the updated master table to the client
void send_master_table(int sock) {
    // Send the number of nodes in the table
    send(sock, &node_count, sizeof(int), 0);

    // Send the master table data
    for (int i = 0; i < node_count; i++) {
        send(sock, &master_table[i], sizeof(NodeInfo), 0);
    }
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    int bytes_received;

    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Setup server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

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

    // Accept client connections and update the master table
    while ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) > 0) {
        printf("New connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Receive client information (Node Number, IP Address, Port Number)
        bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
        if (bytes_received > 0) {
            int node_number;
            int port;
            char client_ip[INET_ADDRSTRLEN];

            // Extract client information from received data
            sscanf(buffer, "%d %s %d", &node_number, client_ip, &port);

            // Update the master table with client information
            update_master_table(node_number, client_ip, port);
            printf("Updated master table with node: %d, IP: %s, Port: %d\n", node_number, client_ip, port);
        }

        // Send the updated master table to the client
        send_master_table(client_fd);

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

